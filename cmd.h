#ifndef _CMD_H
#define _CMD_H

#define con_send(X) __io_putchar(X)
#define con_recv() __io_getchar()

#define COM_MAX_LEN 32
#define ENTER_CHAR 0x0d
#ifndef NULL
#define NULL 0
#endif

struct command{
     unsigned char * cmd_name;
     void (*cmd_fun)(unsigned char *);
};

void go(unsigned char *p);
void print_mem(unsigned char *p);
void print_help(unsigned char *para);
void read_mem(unsigned char *para);
void write_mem(unsigned char *para);
void run_cmd_interface();

#endif
