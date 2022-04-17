#ifndef _FS_H
#define _FS_H
#include "common.h"

#define FS_OK 0
#define FS_FAIL -1

typedef SD_Error (*block_read_func)(uint8_t* pBuffer, uint64_t block_n, uint16_t BlockSize);

int get_file_content(char* buf, const char*filename, uint32_t file_offset, uint32_t len, block_read_func SD_ReadBlock);
#endif
