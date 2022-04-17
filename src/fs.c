#include "fs.h"

#define FS_BUF_SIZE 512
char* fs_buf = NULL;
extern u8 mem_sd_buf[MEM_SD_SIZE];
FATFS g_fat32={0};
FIL g_file={0};
FIL * g_fp=NULL;

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

int get_file_content(char* buf, const char*filename, uint32_t file_offset, uint32_t len, block_read_func rd_block)
{
    BYTE fmt;
    DWORD bsect=0, sysect, nclst, szbfat;
    if(!fs_buf){
        fs_buf = (char*)mem_sd_buf;
    }
    slprintf(buf, "fn:%s off:%d len:%d under developing", filename, file_offset, len);
    if(SD_RESPONSE_NO_ERROR != rd_block((u8*)fs_buf, bsect, FS_BUF_SIZE)){
        lprintf("read disk err\n");
        return FS_DISK_ERR;
    }
    if(g_fp == NULL){
        g_fp = &g_file;
        g_fp->fs = &g_fat32;
        g_fp->fs->rd_block = rd_block;
        if (get_uint_offset(fs_buf, BS_55AA, 2) != 0xAA55)	/* Check boot record signature (always placed at offset 510 even if the sector size is >512) */
            return FS_PART_ERR;
        if (get_uint_offset(fs_buf, BS_FilSysType32, 3) != 0x544146)	/* Check boot record signature (always placed at offset 510 even if the sector size is >512) */
            return FS_NOT_FAT32;
        g_fp->fs->ssize=get_uint_offset(fs_buf, BPB_BytsPerSec, 2);
        lprintf("sector size %x\n", (DWORD)g_fp->fs->ssize);
        g_fp->fs->fsize=get_uint_offset(fs_buf, BPB_FATSz32, 2);
        lprintf("sectors per FAT %x\n", (DWORD)g_fp->fs->fsize);
        g_fp->fs->n_fats=get_uint_offset(fs_buf, BPB_NumFATs, 1);
        lprintf("FATs %x\n", (DWORD)g_fp->fs->n_fats);
        g_fp->fs->csize=get_uint_offset(fs_buf, BPB_SecPerClus, 1);
        lprintf("sectors per cluster %x\n", (DWORD)g_fp->fs->csize);
        g_fp->fs->n_rootdir=get_uint_offset(fs_buf, BPB_RootEntCnt, 2);
        if (g_fp->fs->n_rootdir % (g_fp->fs->ssize / SZ_DIRE))		/* (Must be sector aligned) */
            return FS_NO_FILESYSTEM;
        lprintf("root dir entries %x\n", (DWORD)g_fp->fs->n_rootdir);
        g_fp->fs->tsect=get_uint_offset(fs_buf, BPB_TotSec32, 4);
        lprintf("total sectors %x\n", (DWORD)g_fp->fs->tsect);
        g_fp->fs->nrsv=get_uint_offset(fs_buf, BPB_RsvdSecCnt, 2);
        lprintf("reserved sectors %x\n", (DWORD)g_fp->fs->nrsv);
        if (!g_fp->fs->nrsv) return FS_NO_FILESYSTEM;					/* (Must not be 0) */
        sysect = g_fp->fs->nrsv + g_fp->fs->fsize*g_fp->fs->n_fats + g_fp->fs->n_rootdir / (g_fp->fs->ssize / SZ_DIRE);	/* RSV + FAT + DIR */
        lprintf("data sectors %x\n", sysect);
        if (g_fp->fs->tsect < sysect) return FS_NO_FILESYSTEM;		/* (Invalid volume size) */
        nclst = (g_fp->fs->tsect - sysect) / g_fp->fs->csize;				/* Number of clusters */
        if (!nclst) return FS_NO_FILESYSTEM;				/* (Invalid volume size) */
        fmt = FS_FAT12;
        if (nclst >= MIN_FAT16) fmt = FS_FAT16;
        if (nclst >= MIN_FAT32) fmt = FS_FAT32;
        if(fmt != FS_FAT32)return FS_NOT_FAT32;

        /* Boundaries and Limits */
        g_fp->fs->n_fatent = nclst + 2;							/* Number of FAT entries */
        g_fp->fs->volbase = bsect;								/* Volume start sector */
        g_fp->fs->fatbase = bsect + g_fp->fs->nrsv; 						/* FAT start sector */
        g_fp->fs->database = bsect + sysect;						/* Data start sector */
        if (fmt == FS_FAT32) {
            if (g_fp->fs->n_rootdir) return FS_NO_FILESYSTEM;		/* (BPB_RootEntCnt must be 0) */
            g_fp->fs->dirbase = get_uint_offset(fs_buf, BPB_RootClus, 4);	/* Root directory start cluster */
            szbfat = g_fp->fs->n_fatent * 4;						/* (Needed FAT size) */
        } else {
#if 0
            if (!fs->n_rootdir)	return FR_NO_FILESYSTEM;	/* (BPB_RootEntCnt must not be 0) */
            fs->dirbase = fs->fatbase + fasize;				/* Root directory start sector */
            szbfat = (fmt == FS_FAT16) ?					/* (Needed FAT size) */
                fs->n_fatent * 2 : fs->n_fatent * 3 / 2 + (fs->n_fatent & 1);
#endif
        }
        if (g_fp->fs->fsize < (szbfat + (g_fp->fs->ssize - 1)) / g_fp->fs->ssize)	/* (BPB_FATSz must not be less than the size needed) */
            return FS_NO_FILESYSTEM;

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
        g_fp->fs->fs_type = fmt;	/* FAT sub-type */
#if _FS_RPATH
        fs->cdir = 0;		/* Set current directory to root */
#endif
#if _FS_LOCK			/* Clear file lock semaphores */
        clear_lock(fs);
#endif
        lprintf("fat entries:%d volbase:0x%x FATsect:0x%x data_sect:0x%x dir_clust:0x%x\n",
                g_fp->fs->n_fatent,
                g_fp->fs->volbase,
                g_fp->fs->fatbase,
                g_fp->fs->database,
                g_fp->fs->dirbase);
        //find root dir sec
        bsect = g_fp->fs->database + (g_fp->fs->dirbase - 2) * g_fp->fs->csize;
        if(SD_RESPONSE_NO_ERROR != rd_block((u8*)fs_buf, bsect, FS_BUF_SIZE)){
            lprintf("read disk err\n");
            return FS_DISK_ERR;
        }
        //find filename
        {
            DWORD items = 0;
            while(1){
                if(fs_buf[items*SZ_DIRE] == 0 || items>0xf){
                    lprintf("end of root, not found file!\n");
                    return FS_FILE_NOT_FOUND;
                }
                if(!strncmp(fs_buf+items*SZ_DIRE, "BOOK", 4) &&
                    !strncmp(fs_buf+items*SZ_DIRE+4, "    ", 4) &&
                    !strncmp(fs_buf+items*SZ_DIRE+8, "TXT", 3)){
                    lprintf("found book.txt\n");
                    g_fp->fsize = get_uint_offset(fs_buf+items*SZ_DIRE, DIR_FileSize, 4);
                    lprintf("filesize %d\n", (DWORD)g_fp->fsize);
                    g_fp->sclust = get_uint_offset(fs_buf+items*SZ_DIRE, DIR_FstClusHI, 2);
                    g_fp->sclust <<= 16;
                    g_fp->sclust += get_uint_offset(fs_buf+items*SZ_DIRE, DIR_FstClusLO, 2);
                    lprintf("start clust 0x%x\n", (DWORD)g_fp->sclust);
                    bsect = g_fp->fs->database + (g_fp->sclust - 2) * g_fp->fs->csize;
                    lprintf("start sect 0x%x\n", (DWORD)bsect);
                    if(SD_RESPONSE_NO_ERROR != rd_block((u8*)buf, bsect, FS_BUF_SIZE)){
                        lprintf("read disk err\n");
                        return FS_DISK_ERR;
                    }
                    break;
                }
                items++;
            }
        }
    }
    return FS_OK;
}
