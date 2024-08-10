#include "fs.h"

#define FS_BUF_SIZE 512
char disk_buf[FS_BUF_SIZE];
char* fs_buf = NULL;
FATFS g_fat32={0};
FIL g_file={0};
FIL * g_fp=NULL;
FATFS* g_fs;
int debug_fs = 0;
int disk_retry = 8;

disk_opers g_dops;
#define FAT_cache_N 2
#define FAT_cache_SIZE 8

#define CDB lprintf("line %d\n", __LINE__)
uint32_t FAT_cache[2][8];

void fs_hw_init(disk_opers * in_)
{
    g_dops = *in_;
}

int fs_debug_is_enabled()
{
    return 0;
#if 0
    char t[ENV_MAX_VALUE_LEN];
    if(ENV_OK == get_env("fs_debug", t)){
        //lprintf("getenv fs_debug %s\n", t);
        if(t[0] == '1'){
            return 1;
        }
        else{
            return 0;
        }
    }
    else{
            return 0;
    }
#endif
}

uint32_t get_uint_offset(char* buf_base, uint32_t off, uint32_t num)
{
    uint32_t ret = 0;
    char* pos = buf_base + off + num - 1;
    while(num--){
        ret <<= 8;
        ret += *pos;
        pos--;
    }
    return ret;
}

int recover_sd()
{
    lprintf("try re init sd\n");
    if(SD_OK != g_fs->disk_init()){
        lprintf("fail, try repower\n");
        SD_repower();
        if(SD_OK != g_fs->disk_init()){
            lprintf("fail, abandon\n");
            return 0;
        }
        else{
            lprintf("OK, retry\n");
            return 1;
        }
    }
    else{
        lprintf("OK, retry\n");
        return 1;
    }
}

static uint32_t current_r_sector_no = 0xffffffff;

const char* disk_write_sector(const char*buf, uint32_t sector_no)
{
    int retry = disk_retry;
    if(current_r_sector_no == sector_no){
        current_r_sector_no = 0xffffffff;
    }
    while(1){
        if(SD_RESPONSE_NO_ERROR == g_fs->wt_block((u8*)buf, sector_no, FS_BUF_SIZE)){
            return buf;
        }
        else{
            lprintf("write disk err retry=%d\n", retry);
        }
        if(retry--==0){
            lprintf("FATAL:!!!!!!!!!!!!write disk err secno:%d 0x%x\n", sector_no, sector_no);

            if(recover_sd()){
                lprintf("OK, retry\n");
                retry = disk_retry;
            }
            else{
                lprintf("fail, abandon\n");
                return NULL;
            }
        }
    }
    return NULL;
}

char* disk_read_sector(uint32_t sector_no)
{
    int retry = disk_retry;
    if(current_r_sector_no == sector_no){
        //lprintf("just read\n");
        return disk_buf;
    }
    while(1){
        if(SD_RESPONSE_NO_ERROR != g_fs->rd_block((u8*)disk_buf, sector_no, FS_BUF_SIZE)){
            lprintf("read disk err, retry=%d\n", retry);
        }
        else{
            break;
        }
        if(retry--==0){
            lprintf("FATAL:!!!!!!!!!!!!read disk err secno:%d 0x%x\n", sector_no, sector_no);
            if(recover_sd()){
                lprintf("OK, retry\n");
                retry = disk_retry;
            }
            else{
                lprintf("fail, abandon\n");
                return NULL;
            }
        }
    }
    current_r_sector_no = sector_no;
    //lprintf("read 0x%x sector OK\n", sector_no);
    if(debug_fs)mem_print(disk_buf, 512*sector_no, 512);
    return disk_buf;
}

uint32_t find_in_fat_cach(uint32_t cn)
{
    int i, j;
    uint32_t ret = INVALID_CLUSTER;
    if(debug_fs)lprintf("find_in_fat_cach %X\n", cn);
    if(debug_fs)mem_print((const char*)FAT_cache, 0, 2*8*4);
    for(i = 0; i < FAT_cache_N; i++){
        for(j = 0; j < FAT_cache_SIZE; j++){
            if(debug_fs)lprintf("%X ", FAT_cache[i][j]);
            if(FAT_cache[i][j] == 0){
                break;
            }
            if(FAT_cache[i][j] == cn){
                ret = (i<<16)+j;
                if(debug_fs)lprintf("ret1:%X\n", ret);
                return ret;
            }
        }
        if(debug_fs)lprintf("\n");
    }
    if(debug_fs)lprintf("ret2:%X\n", ret);
    return ret;
}

void save_fat_cache(uint32_t cn, uint32_t next)
{
    uint32_t ret = INVALID_CLUSTER;
    int i, j;
    ret = find_in_fat_cach(cn);
    if(ret == INVALID_CLUSTER){
        for(i = 0; i < FAT_cache_N; i++){
            if(FAT_cache[i][0] == 0){
                FAT_cache[i][0] = cn;
                FAT_cache[i][1] = next;
                if(debug_fs)lprintf("save fatcache 0x:%x %x@head %x OK\n",
                        cn, next, i);
                return;
            }
        }
    }
    else{
        j = ret&0xffff;
        if(j<FAT_cache_SIZE-1){
            i = (ret&0xffff0000)>>16;
            FAT_cache[i][j+1] = next;
            if(debug_fs)lprintf("save fatcache 0x:%x %x@%x %x OK\n",
                    cn, next, i, j+1);
            return;
        }
    }
    if(debug_fs)lprintf("save fatcache 0x:%x %x fail\n",
                    cn, next);
}

uint32_t get_fat_cache(uint32_t cn)
{
    int i, j;
    uint32_t ret = INVALID_CLUSTER;
    uint32_t find;
    find = find_in_fat_cach(cn);
    if(debug_fs)lprintf("find %X\n", find);
    if(find != INVALID_CLUSTER){
        j = find&0xffff;
        if(debug_fs)lprintf("j %X\n", j);
        if(j<FAT_cache_SIZE-1){
            i = (find&0xffff0000)>>16;
            if(debug_fs)lprintf("i %X\n", i);
            ret = FAT_cache[i][j+1];
            if(debug_fs)lprintf("found fatcache:0x%x->0x%x\n",
                    cn, ret);
        }
    }
    return ret;
}

uint32_t get_next_cluster(uint32_t cluster_no)
{
    void*p;
    uint32_t next;
    next = get_fat_cache(cluster_no);
    if(next != INVALID_CLUSTER && next != 0){
        return next;
    }
    uint32_t cluster_fat_offset = SZ_FAT_CLUSTER*cluster_no;
    uint32_t sectors_ct = cluster_fat_offset/g_fs->ssize;
    uint32_t sector_offset = cluster_fat_offset%g_fs->ssize;
    if(debug_fs)lprintf("fat base:%X sct %X off %X\n", g_fs->fatbase, sectors_ct, sector_offset);
    p = disk_read_sector(g_fs->fatbase+sectors_ct);
    if(p != NULL){
        next = *(uint32_t*)(p+sector_offset);
        save_fat_cache(cluster_no, next);
        return next;
    }
    else{
        return INVALID_CLUSTER;
    }
}

uint32_t get_later_cluster(uint32_t start_clust_no, uint32_t clusters_count)
{
    uint32_t next_cluster = start_clust_no;
    if(debug_fs)lprintf("%s+0x%x 0x%x\n", __func__, start_clust_no, clusters_count);
    while(clusters_count--){
        if(debug_fs)lprintf("next:%x->", next_cluster);
        next_cluster = get_next_cluster(next_cluster);
        if(debug_fs)lprintf("%x\n", next_cluster);
        if(next_cluster > MIN_EOF){
            lprintf("End cluster\n");
            break;
        }
    }
    if(debug_fs)lprintf("%s-0x%x\n", __func__, next_cluster);
    return next_cluster;
}

char* get_clust_offset_buf(uint32_t cluster_no, uint32_t offset, char*cp_buf, uint32_t len)
{
    char *reader;
    if(len == 0){
        return cp_buf;
    }
    uint32_t clust_bytes = g_fp->fs->csize * g_fs->ssize;
    uint32_t cluster_off = offset % clust_bytes;
    uint32_t target_sector_no = g_fp->fs->database + (cluster_no - 2) * g_fs->csize;
    uint32_t sector_off_in_cluster = cluster_off/g_fs->ssize;
    uint32_t sector_off = cluster_off % g_fs->ssize;
    target_sector_no += sector_off_in_cluster;
    //lprintf("target sec %x\n", target_sector_no);
    reader = disk_read_sector(target_sector_no);
    if(reader==NULL){
        return NULL;
    }
    reader += sector_off;
    if(cp_buf != NULL){
        char* writer = cp_buf;
        uint32_t left_in_sector = g_fs->ssize - sector_off;
        //copy the data
        while(1){
            uint32_t cp_len = len>left_in_sector?left_in_sector:len;
            memcpy(writer, reader, cp_len);
            len -= cp_len;
            writer += cp_len;
            if(len>0){
                if(sector_off_in_cluster < g_fs->csize){
                    target_sector_no ++;
                    sector_off_in_cluster++;
                    reader = disk_read_sector(target_sector_no);
                    if(reader==NULL){
                        return NULL;
                    }
                    left_in_sector = g_fs->ssize;
                }
                else{
                    break;
                }
            }
            else{
                break;
            }
        }
        //CDB;
        return cp_buf;
    }
    else{
        return reader;
    }
}

char* get_file_offset_buf(uint32_t start_clust_no, uint64_t offset, char*cp_buf, uint64_t len)
{
    char*ret=cp_buf;
    uint32_t clust_bytes = g_fp->fs->csize * g_fs->ssize;
    uint32_t off_clusters =  offset / clust_bytes;
    uint32_t cluster_off = offset % clust_bytes;
    uint32_t target_cluster_no = get_later_cluster(start_clust_no, off_clusters);
    if(debug_fs)lprintf("tcn %d st %d offcl %d cloff %d\n",
            target_cluster_no, start_clust_no, off_clusters, cluster_off);
    if(target_cluster_no > MIN_EOF){
        return NULL;
    }
    if(cp_buf==NULL){
        return get_clust_offset_buf(target_cluster_no, cluster_off, NULL, len);
    }
    else{
        uint32_t cluster_left_bytes = clust_bytes-cluster_off;
        //lprintf("left %d len %X%X\n", cluster_left_bytes, len);
        if(cluster_left_bytes >= len){
            return get_clust_offset_buf(target_cluster_no, cluster_off, cp_buf, len);
        }
        else{
            char*writer = cp_buf;
            uint64_t read_len;
            while(1){
                read_len = cluster_left_bytes<len?cluster_left_bytes:len;
                //lprintf("-redlen %X%X\n", read_len);
                get_clust_offset_buf(target_cluster_no, cluster_off, writer, read_len);
                len -= read_len;
                //lprintf("-len %X%X\n", len);
                if(len==0){
                    return ret;
                }
                writer += read_len;
                cluster_off = 0;
                target_cluster_no = get_later_cluster(target_cluster_no , 1);
                cluster_left_bytes = clust_bytes;
                //lprintf("tcn %x\n", target_cluster_no);
                if(target_cluster_no > MIN_EOF){
                    return NULL;
                }
            }
        }
    }
}

char* get_root_item_buf(int n)
{
    return get_file_offset_buf(g_fs->dirbase, SZ_DIRE*n, NULL, SZ_DIRE);
}

uint32_t get_file_start_cluster(const char* filename, const char*fileextname)
{
    char*item_buf;
    uint32_t file_start_cluster = INVALID_CLUSTER, namelen, extnamelen;
    namelen=strlen(filename);
    extnamelen=strlen(fileextname);
    //find filename
    DWORD items = 0;
    while(1){
        //lprintf("dir items 0x%x\n", items);
        item_buf=get_root_item_buf(items);
        if(item_buf == 0 || *item_buf == 0){
            lprintf("end of root, not found file!\n");
            break;
        }
        if(debug_fs)mem_print(item_buf, 0, 32);
        if(!strncmp(item_buf, filename, namelen) &&
                !strncmp(item_buf+namelen, "    ", 8-namelen) &&
                !strncmp(item_buf+8, fileextname, extnamelen)){
            lprintf("found %s.%s\n", filename, fileextname);
            g_fp->fsize = get_uint_offset(item_buf, DIR_FileSize, 4);
            lprintf("filesize %d\n", (DWORD)g_fp->fsize);
            file_start_cluster = get_uint_offset(item_buf, DIR_FstClusHI, 2);
            file_start_cluster <<= 16;
            file_start_cluster += get_uint_offset(item_buf, DIR_FstClusLO, 2);
            break;
        }
        items++;
    }
    return file_start_cluster;
}

#define MBR_FS_TYPE 0x1c2
#define MBR_FIRST_PART_OFFSET 0x1c6
int init_fs()
{
    BYTE fmt;
    DWORD bsect=0, sysect, nclst, szbfat;
    lprintf("init fs\n");
    g_fs = &g_fat32;
    if(g_dops.disk_hw_inited==FS_HW_INITED){
        g_fs->rd_block = g_dops.rd_block;
        g_fs->wt_block = g_dops.wt_block;
        g_fs->disk_init = g_dops.disk_init;
    }
    else{
        return FS_DISK_ERR;
    }
    memset(&FAT_cache[0][0], 0, FAT_cache_N*FAT_cache_SIZE);
    if(NULL == disk_read_sector(bsect)){
        lprintf("read disk err\n");
        return FS_DISK_ERR;
    }
dbr_check:
    /* Check boot record signature (always placed at offset 510 even if the sector size is >512) */
    if (get_uint_offset(fs_buf, BS_55AA, 2) != 0xAA55){
        lprintf("last is not AA55\n");
        return FS_PART_ERR;
    }
    /* Check boot record signature (always placed at offset 510 even if the sector size is >512) */
    if (get_uint_offset(fs_buf, BS_FilSysType32, 3) != 0x544146){
        lprintf("MBR\n");
        if(get_uint_offset(fs_buf, MBR_FS_TYPE, 1) == 0x0c){
            lprintf("Win95 FAT32\n");
        }
        bsect += get_uint_offset(fs_buf, MBR_FIRST_PART_OFFSET, 4);
        lprintf("Read DBR\n");
        if(NULL == disk_read_sector(bsect)){
            lprintf("read disk err\n");
            return FS_DISK_ERR;
        }
        goto dbr_check;
    }
    lprintf("DBR\n");
    g_fs->ssize=get_uint_offset(fs_buf, BPB_BytsPerSec, 2);
    lprintf("sector size %x\n", (DWORD)g_fp->fs->ssize);
    g_fs->fsize=get_uint_offset(fs_buf, BPB_FATSz32, 2);
    lprintf("sectors per FAT %x\n", (DWORD)g_fs->fsize);
    g_fs->n_fats=get_uint_offset(fs_buf, BPB_NumFATs, 1);
    lprintf("FATs %x\n", (DWORD)g_fs->n_fats);
    g_fs->csize=get_uint_offset(fs_buf, BPB_SecPerClus, 1);
    lprintf("sectors per cluster %x\n", (DWORD)g_fs->csize);
    g_fs->n_rootdir=get_uint_offset(fs_buf, BPB_RootEntCnt, 2);
    if (g_fs->n_rootdir % (g_fs->ssize / SZ_DIRE))		/* (Must be sector aligned) */
    {
        lprintf("sector not aligned\n");
        return FS_NO_FILESYSTEM;
    }
    lprintf("root dir entries %x\n", (DWORD)g_fs->n_rootdir);
    g_fs->tsect=get_uint_offset(fs_buf, BPB_TotSec32, 4);
    lprintf("total sectors %x\n", (DWORD)g_fs->tsect);
    g_fs->nrsv=get_uint_offset(fs_buf, BPB_RsvdSecCnt, 2);
    lprintf("reserved sectors %x\n", (DWORD)g_fs->nrsv);
    if (!g_fs->nrsv){
        lprintf("nrsv is not 0\n");
        return FS_NO_FILESYSTEM;					/* (Must not be 0) */
    }
    sysect = g_fs->nrsv + g_fs->fsize*g_fs->n_fats + g_fs->n_rootdir / (g_fs->ssize / SZ_DIRE);	/* RSV + FAT + DIR */
    lprintf("data sectors %x\n", sysect);
    if (g_fs->tsect < sysect)
    {
        lprintf("invalid volume size\n");
        return FS_NO_FILESYSTEM;		/* (Invalid volume size) */
    }
    nclst = (g_fs->tsect - sysect) / g_fs->csize;				/* Number of clusters */
    if (!nclst) return FS_NO_FILESYSTEM;				/* (Invalid volume size) */
    fmt = FS_FAT12;
    if (nclst >= MIN_FAT16) fmt = FS_FAT16;
    if (nclst >= MIN_FAT32) fmt = FS_FAT32;
    if(fmt != FS_FAT32){
        lprintf("is not fat32\n");
        return FS_NOT_FAT32;
    }

    /* Boundaries and Limits */
    g_fs->n_fatent = nclst + 2;							/* Number of FAT entries */
    g_fs->volbase = bsect;								/* Volume start sector */
    g_fs->fatbase = bsect + g_fs->nrsv; 						/* FAT start sector */
    g_fs->database = bsect + sysect;						/* Data start sector */
    if (fmt == FS_FAT32) {
        if (g_fs->n_rootdir){
            lprintf("BPB_RootEntCnt is not 0\n");
            return FS_NO_FILESYSTEM;		/* (BPB_RootEntCnt must be 0) */
        }
        g_fs->dirbase = get_uint_offset(fs_buf, BPB_RootClus, 4);	/* Root directory start cluster */
        szbfat = g_fs->n_fatent * 4;						/* (Needed FAT size) */
    } else {
        lprintf("2:is not fat32\n");
        return FS_NOT_FAT32;
    }
    if (g_fs->fsize < (szbfat + (g_fs->ssize - 1)) / g_fs->ssize)	/* (BPB_FATSz must not be less than the size needed) */
    {
        lprintf("BPB_FATSz must not be less than the size needed\n");
        return FS_NO_FILESYSTEM;
    }

#if _FS_WRITE
    /* Initialize cluster allocation information */
    fs->last_clust = fs->free_clust = 0xFFFFFFFF;

    /* Get fsinfo if available */
    fs->fsi_flag = 0x80;
#if (_FS_NOFSINFO & 3) != 3
    if (fmt == FS_FAT32				/* Enable FSINFO only if FAT32 and BPB_FSInfo is 1 */
            && LD_WORD(fs->win.d8 + BPB_FSInfo) == 1
            && move_window(fs, bsect + 1) == FR_OK)
    {
        fs->fsi_flag = 0;
        if (LD_WORD(fs->win.d8 + BS_55AA) == 0xAA55	/* Load FSINFO data if available */
                && LD_DWORD(fs->win.d8 + FSI_LeadSig) == 0x41615252
                && LD_DWORD(fs->win.d8 + FSI_StrucSig) == 0x61417272)
        {
#if (_FS_NOFSINFO & 1) == 0
            fs->free_clust = LD_DWORD(fs->win.d8 + FSI_Free_Count);
#endif
#if (_FS_NOFSINFO & 2) == 0
            fs->last_clust = LD_DWORD(fs->win.d8 + FSI_Nxt_Free);
#endif
        }
    }
#endif
#endif
    g_fs->fs_type = fmt;	/* FAT sub-type */
#if _FS_RPATH
    fs->cdir = 0;		/* Set current directory to root */
#endif
#if _FS_LOCK			/* Clear file lock semaphores */
    clear_lock(fs);
#endif
    lprintf("fat entries:%d volbase:0x%x FATsect:0x%x data_sect:0x%x dir_clust:0x%x\n",
            g_fs->n_fatent,
            g_fs->volbase,
            g_fs->fatbase,
            g_fs->database,
            g_fs->dirbase);
    return FS_OK;
}

int get_file_size(const char*filename, const char*ext_filename)
{
    int ret;
    lprintf("get_file_size %s.%s\n", filename, ext_filename);
    /*
    if(fs_debug_is_enabled()){
        debug_fs = 1;
    }
    */
    if(!fs_buf){
        fs_buf = (char*)disk_buf;
    }
    if(NULL == g_fs || g_fs->fs_type != FS_FAT32){
        ret = init_fs();
        if(FS_OK != ret){
            return ret;
        }
    }
    if(g_fp == NULL)
    {
        g_fp = &g_file;
        g_fp->sclust=INVALID_CLUSTER;
        g_fp->fs = g_fs;
    }
    if(g_fp->sclust==INVALID_CLUSTER){
        g_fp->sclust = get_file_start_cluster(filename, ext_filename);
    }
    if(g_fp->sclust!=INVALID_CLUSTER){
        //find root dir sec
        lprintf("file start clust 0x%x size %d\n", (DWORD)g_fp->sclust, g_fp->fsize);
        g_fp->in_writing = 0;
        g_fp->fptr = 0;
        return g_fp->fsize;
    }
    else{
        return FS_FILE_NOT_FOUND;
    }
}

int get_file_content(char* buf, const char*filename, const char*extfn, uint32_t file_offset, uint32_t len)
{
    int ret;
    lprintf("get_file_content: fileoff %d len %d\n", file_offset, len);
    slprintf(buf, "fn:%s off:%d len:%d under developing", filename, file_offset, len);
    ret = get_file_size(filename, extfn);
    if(0 < ret){
        if(NULL == get_file_offset_buf(g_fp->sclust, file_offset, buf, len)){
            return FS_FILE_NOT_FOUND;
        }
        else{
            return FS_OK;
        }
    }
    else{
        return ret;
    }
}

int open_file_for_write(const char*fn, const char*ext)
{
    int ret;
    ret = get_file_size(fn, ext);
    if(0 < ret){
        g_fp->clust = g_fp->sclust;
        g_fp->clust_sec_offset = 0;
        g_fp->in_writing = 1;
        return FS_OK;
    }
    else{
        return ret;
    }
}

void close_file()
{ 
    if(g_fp != NULL){
        g_fp->clust = 0;
        g_fp->clust_sec_offset = 0;
        g_fp->in_writing = 0;
        g_fp->sclust=INVALID_CLUSTER;
    }
}

int write_sec_to_file(const char*buf)
{
    if(!g_fp->in_writing){
        lprintf("try write not opened file\r\n");
        return -1;
    }
    //lprintf("clust %d sec_off %d\r\n", g_fp->clust, g_fp->clust_sec_offset);
    uint32_t target_sector_no = g_fp->fs->database + (g_fp->clust - 2) * g_fs->csize;
    target_sector_no += g_fp->clust_sec_offset;
    if(NULL==disk_write_sector(buf, target_sector_no)){
        return -1;
    }
    g_fp->clust_sec_offset++;
    if(g_fp->clust_sec_offset >=g_fp->fs->csize)
    {
        g_fp->clust = get_next_cluster(g_fp->clust);
        g_fp->clust_sec_offset = 0;
    }
    g_fp->fptr += g_fs->ssize;
    return g_fp->fptr;
}
