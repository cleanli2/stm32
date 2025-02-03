#include "fs.h"

#define FS_BUF_SIZE 512
char disk_buf[FS_BUF_SIZE];
char* fs_buf = NULL;
FATFS g_fat32={0};
FIL g_file={0};
FIL * g_fp=NULL;
FATFS* g_fs;
static int debug_fs = -1;
static int file_opened_no = 9;

#define FAT_cache_SIZE 64

#define CDB lprintf("line %d\n", __LINE__)
uint32_t file_fat_cach_start_clno = 0xffffffff;
uint32_t file_fat_cach_base = -1;
uint32_t FAT_cache[FAT_cache_SIZE];
uint32_t get_next_cluster(uint32_t cluster_no);

int fs_debug_is_enabled()
{
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

char* disk_read_sector(uint32_t sector_no)
{
    static uint32_t current_sector_no = 0xffffffff;
    if(current_sector_no == sector_no){
        return disk_buf;
    }
    if(SD_RESPONSE_NO_ERROR != g_fs->rd_block((u8*)disk_buf, sector_no, FS_BUF_SIZE)){
        lprintf("read disk err\n");
        return NULL;
    }
    current_sector_no = sector_no;
    if(debug_fs) lprintf("read 0x%x sector OK\n", sector_no);
    if(debug_fs)mem_print(disk_buf, 512*sector_no, 512);
    return disk_buf;
}

void save_fat_cache(uint32_t cl_base, uint32_t cn, uint32_t cn_cl)
{
    uint32_t*fat_ch_p=FAT_cache;
    uint32_t next_cluster = cn_cl;
    uint32_t cach_clusters_count=FAT_cache_SIZE;
    memset((unsigned char*)FAT_cache, 0xff, FAT_cache_SIZE*4);
    file_fat_cach_base = cl_base;
    file_fat_cach_start_clno = cn;
    *fat_ch_p++=next_cluster;
    cach_clusters_count--;
    while(cach_clusters_count--){
        if(debug_fs)lprintf("next:%x->", next_cluster);
        next_cluster = get_next_cluster(next_cluster);
        if(debug_fs)lprintf("%x\n", next_cluster);
        if(next_cluster > MIN_EOF){
            lprintf("End cluster\n");
            break;
        }
        *fat_ch_p++=next_cluster;
    }
    if(debug_fs)lprintf("save fatcache start from 0x:%x done\n", cn);
}

uint32_t get_fat_cache(uint32_t cache_cl_base, uint32_t cn)
{
    uint32_t ret = INVALID_CLUSTER;
    if(cache_cl_base!=file_fat_cach_base){
        return ret;
    }

    if(0xffffffff==file_fat_cach_start_clno){
        if(debug_fs)lprintf("no fat_cach\n");
        return ret;
    }
    if(debug_fs)mem_print((const char*)FAT_cache, 0, FAT_cache_SIZE*4);
    if(FAT_cache_SIZE>cn-file_fat_cach_start_clno){
        ret = FAT_cache[cn-file_fat_cach_start_clno];
    }
    else{
        lprintf("NinCach\n");
    }
    if(debug_fs)lprintf("ret2:%X\n", ret);
    return ret;
}

uint32_t get_next_cluster(uint32_t cluster_no)
{
    void*p;
    uint32_t next;
    uint32_t cluster_fat_offset = SZ_FAT_CLUSTER*cluster_no;
    uint32_t sectors_ct = cluster_fat_offset/g_fs->ssize;
    uint32_t sector_offset = cluster_fat_offset%g_fs->ssize;
    if(debug_fs)lprintf("fat base:%X sct %X off %X\n", g_fs->fatbase, sectors_ct, sector_offset);
    p = disk_read_sector(g_fs->fatbase+sectors_ct);
    if(p != NULL){
        next = *(uint32_t*)(p+sector_offset);
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

    uint32_t target_cluster_no = get_fat_cache(start_clust_no, off_clusters);
    if(target_cluster_no != INVALID_CLUSTER && ret != 0){
        //OK, do nothing
    }
    else{
        if(file_fat_cach_start_clno != 0xffffffff
                && start_clust_no==file_fat_cach_base
                && target_cluster_no>=file_fat_cach_start_clno+FAT_cache_SIZE){
            target_cluster_no = get_later_cluster(FAT_cache[FAT_cache_SIZE-1],
                    off_clusters-(file_fat_cach_start_clno+FAT_cache_SIZE-1));

        }
        else{
            target_cluster_no = get_later_cluster(start_clust_no, off_clusters);
        }
        if(debug_fs)lprintf("tcn %d st %d offcl %d cloff %d\n",
                target_cluster_no, start_clust_no, off_clusters, cluster_off);
        if(target_cluster_no > MIN_EOF){
            return NULL;
        }
        if(NULL!=cp_buf){//find file name no need save fat cache
            save_fat_cache(start_clust_no, off_clusters, target_cluster_no);
        }
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
        lprintf("dir items 0x%x\n", items);
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
int init_fs(block_read_func rd_block)
{
    BYTE fmt;
    DWORD bsect=0, sysect, nclst, szbfat;
    lprintf("init fs\n");
    g_fs = &g_fat32;
    g_fs->rd_block = rd_block;
    memset(&FAT_cache[0], 0xff, 4*FAT_cache_SIZE);
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

int get_file_size(block_read_func rd_block, const char*fn, const char*en)
{
    int ret;
    //lprintf("get_file_size+\n");
    if(-1 == debug_fs){
        if(fs_debug_is_enabled()){
            debug_fs = 1;
        }
        else{
            debug_fs = 0;
        }
    }
    if(!fs_buf){
        fs_buf = (char*)disk_buf;
    }
    if(NULL == g_fs || g_fs->fs_type != FS_FAT32){
        if(NULL==rd_block){
            return FS_FILE_PARA_ERR;
        }
        ret = init_fs(rd_block);
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
        if(NULL==fn || NULL==en){
            return FS_FILE_PARA_ERR;
        }
        g_fp->sclust = get_file_start_cluster(fn, en);
        lprintf("file start clust 0x%x size %d\n", (DWORD)g_fp->sclust, g_fp->fsize);
    }
    if(g_fp->sclust!=INVALID_CLUSTER){
        //find root dir sec
        return g_fp->fsize;
    }
    else{
        return FS_FILE_NOT_FOUND;
    }
}

int get_file_content(char* buf, const char*filename, const char*extname, uint32_t file_offset, uint32_t len, block_read_func rd_block)
{
    int ret;
    //lprintf("get_file_content: fileoff %d len %d\n", file_offset, len);
    slprintf(buf, "fn:%s off:%d len:%d under developing", filename, file_offset, len);
    ret = get_file_size(rd_block,filename,extname);
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

int open_file(block_read_func rd_block, const char*fn, const char*en, int*filesize)
{
    int ret;
    if(NULL!=g_fp && g_fp->sclust!=INVALID_CLUSTER){
        return FS_FILE_NOT_CLOSE;
    }
    ret = get_file_size(rd_block, fn, en);
    if( ret < 0){
        lprintf("file open failed\n");
        return ret;
    }
    if(filesize){
        *filesize=ret;
    }
    file_opened_no++;
    return file_opened_no;
}

int close_file_legacy(int fd)
{
    if(file_opened_no!=fd){
        return FS_FILE_NOT_OPEN;
    }
    g_fp->sclust=INVALID_CLUSTER;
    return file_opened_no;
}

int read_file(int fd, char*buf, uint32_t file_offset, uint32_t len)
{
    int ret;
    if(file_opened_no!=fd){
        return FS_FILE_NOT_OPEN;
    }
    if(g_fp->sclust==INVALID_CLUSTER){
        return FS_FILE_NOT_OPEN;
    }
    if(file_offset> g_fp->fsize){
        return FS_FILE_PARA_ERR;
    }
    if(file_offset+len > g_fp->fsize){
        lprintf("Warning:read_file len exceed file, fixed\n");
        len = g_fp->fsize-file_offset;
    }
    ret = get_file_content(buf, NULL, NULL, file_offset, len, NULL);
    if(ret != FS_OK){
        lprintf("sd file read fail\n");
        return ret;
    }
    return ret;
}
