#ifndef __COMM_CTR_H
#define __COMM_CTR_H

#define REQ_INFO 0x81
#define REQ_ACCESS 0x82
#define REQ_UPDATE 0x83
#define REQ_PWN 0x84
#define RSP_ACK 0x8a
#define RSP_NACK 0x9c

typedef struct mupack_t{
    char reqrsp;
    char len;
    char data[0];
} mupk;

#endif
