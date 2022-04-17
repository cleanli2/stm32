#include "fs.h"

#define FS_BUF_SIZE 512
char* fs_buf = NULL;
extern u8 mem_sd_buf[MEM_SD_SIZE];

int get_file_content(char* buf, const char*filename, uint32_t file_offset, uint32_t len, block_read_func rd_block)
{
    if(!fs_buf){
        fs_buf = (char*)mem_sd_buf;
    }
    slprintf(buf, "fn:%s off:%d len:%d under developing", filename, file_offset, len);
    if(SD_RESPONSE_NO_ERROR != rd_block((u8*)fs_buf, 0, FS_BUF_SIZE)){
        lprintf("read disk err\n");
        return FS_FAIL;
    }
    return FS_OK;
}
