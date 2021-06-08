#include "cmd.h"
#include "common.h"
#include "lprintf.h"
#include "sd/stm32_eval_spi_sd.h"
#include <stdint.h>
#define uint uint32_t
#define lprint lprintf

static char cmd_buf[COM_MAX_LEN] = "";
static uint cmd_buf_p = COM_MAX_LEN;
static uint quit_cmd = 0;

uint get_howmany_para(char *s);
char * str_to_hex(char *s, uint * result);
/*
void con_send(char X)
{
    __io_putchar(X);
    if(X == '\n'){
        __io_putchar('\r');
    }
}
*/
uint8_t cmd_caches[40][40] = {0};
uint32_t ci=0;
uint8_t read_buf[512];

#if 1
void sd(char *p)
{
    uint32_t para1 = 0, para2=0, para3 = 0, tmp, cmdindex;
    SD_Error Status = SD_OK;
    SD_CardInfo mycard;

    lprintf("p=%s\n", p);
    strcpy(cmd_caches[ci++], p);
    if(ci > 40)
	    ci = 0;
    tmp = get_howmany_para(p);
    lprintf("tmp=%d\n", tmp);
    if(tmp>=1)
	    p = str_to_hex(p, &cmdindex);
    if(tmp == 0 || cmdindex == 0){
	    lprintf("sd_init\n");
	    if((Status = SD_Init()) != SD_OK)
	    {
		    lprintf("Fail\n");
	    }
	    else{
		    lprintf("OK\n");
		    if((Status = SD_GetCardInfo(&mycard)) != SD_OK)
		    {
			    lprintf("get card info Fail\n");
		    }
		    else{
			    lprintf("block size %d\n", mycard.CardBlockSize);
			    lprintf("block capacity %d\n", mycard.CardCapacity);
		    }
	    }

	    //lprintf("sd_deinit\n");
	    //SD_DeInit();
    }
    else if(cmdindex == 1){//cs high/low
	lprintf("line %d\n", __LINE__);
        str_to_hex(p, &para1);
	if(para1)
		SD_CS_HIGH();
	else
		SD_CS_LOW();
    }
    else if(cmdindex == 2){//
	lprintf("line %d\n", __LINE__);
	SD_WriteByte(SD_DUMMY_BYTE);
    }
    else if(cmdindex == 3){//
        str_to_hex(p, &para1);
	lprintf("line %d para1 %x\n", __LINE__, para1);
	lprintf("read byte %x\n", SD_SPI_ReadWriteByte(para1));
    }
    else if(cmdindex == 4){//
	lprintf("SD_GetRes %x\n", SD_GetRes());
    }
    else if(cmdindex == 5){//cmd
	lprintf("line %d %x %x %x\n", __LINE__, para1, para2, para3);
        p = str_to_hex(p, &para1);
        p = str_to_hex(p, &para2);
        p = str_to_hex(p, &para3);
	SD_SendCmd(para1, para2, para3);
    }
    else if(cmdindex == 6){//cmd
	lprintf("line %d %x %x %x\n", __LINE__, para1, para2, para3);
        p = str_to_hex(p, &para1);
        p = str_to_hex(p, &para2);
        p = str_to_hex(p, &para3);
	lprintf("return %x\n", getres_SD_SendCmd(para1, para2, para3));
    }
    else if(cmdindex == 7){//low init/deinit
        str_to_hex(p, &para1);
	lprintf("line %d para1 %x\n", __LINE__, para1);
	if(para1)
		SD_LowLevel_Init(); 
	else
		SD_LowLevel_DeInit();
    }
    else if(cmdindex == 8){//
	lprintf("do basic Init()\n");
	lprintf("SD_LowLevel_Init()\n");
	/*!< Initialize SD_SPI */
	SD_LowLevel_Init(); 

	/*!< SD chip select high */
	lprintf("SD_CS_HIGH()\n");
	SD_CS_HIGH();

	lprintf("send SD dummy bytes\n");
	/*!< Send dummy byte 0xFF, 10 times with CS high */
	/*!< Rise CS and MOSI for 80 clocks cycles */
	for (int i = 0; i <= 9; i++)
	{
		/*!< Send dummy byte 0xFF */
		SD_WriteByte(SD_DUMMY_BYTE);
	}
    }
    else if(cmdindex == 9){//
        str_to_hex(p, &para1);
	lprintf("line %d para1 %x\n", __LINE__, para1);
	lprintf("read byte %x\n", SD_ReadByte());
    }
    else if(cmdindex == 0xa){//
	lprintf("read 1 block\n");
	SD_ReadBlock(read_buf, 0, 512);
	char*cp = (char *)read_buf;
	int length=512;
	while(length){
		lprint("\n%x: ", (int)cp);
		for(int i=0;i<16;i++){
			length--;
			lprintf("%b", *cp++);
			con_send(i == 7 ? '-':' ');
		}
	}
    }
    con_send('\n');

    return;

error:
    lprint("Err!\ndispcchar [x] [y]\n");
}
void sd_cmds(char *p)
{
    uint tmp = get_howmany_para(p);
    uint para; 
    lprintf("tmp %x\n", tmp);
    if(tmp==0){
	    for(uint i =0;i<40;i++){
		    if(strlen(cmd_caches[i])>0){
			    lprintf("%d:%s\n", i, cmd_caches[i]);
		    }
	    }
    }
    else{
	    p = str_to_hex(p, &para);
	    lprintf("para %x\n", para);
	    switch(para){
		    case 0:
			    lprintf("clear cmd caches\n");
			    memset(*cmd_caches, 0, 1600);
			    ci = 0;
			    break;
		    case 1:
			    memset(cmd_caches[ci], 0, 40);
			    ci--;
			    break;
		    case 2:
			    for(uint i =0;i<40;i++){
				    if(strlen(cmd_caches[i])>0){
					    lprintf("%d:%s\n", cmd_caches[i]);
					    sd(cmd_caches[i]);
				    }
			    }
			    break;
		    default:
			    lprintf("do nothhing\n");
	    }
    }
    con_send('\n');

    return;
}
#endif

void lcdsuebinit(char *p)
{
    uint tmp = get_howmany_para(p);
    uint para = 0; 
    if(tmp>=1){
	    p = str_to_hex(p, &para);
    }
    lprintf("para %x\n", para);
    lcd_sueb_init(para);
    lcd_sueb_test();
    con_send('\n');

    return;

error:
    lprint("Err!\ndispcchar [x] [y]\n");
}

void lcdsuebtest(char *p)
{
    lcd_sueb_test();
    con_send('\n');

    return;

error:
    lprint("Err!\ndispcchar [x] [y]\n");
}

#if 0
void lcd19264init(char *p)
{
  lcd_init();
  Display_Chinese2_12(0,0,ziku12+(0x18*2));//page 0, column 0
  Display_Chinese2_12(0,20,ziku12+(0x18*3));//page 0, column 20
  Display_Chinese_r90(0,40,lei1);//page 0, column 20
  /*
  Display_Chinese2(0,40,ziku);
  Display_Chinese2(0,60,ziku+0x20);
  Display_Chinese2(0,80,ziku+0x40);
  Display_Chinese2(0,100,ziku+0x60);
  Display_Chinese2(0,120,ziku+0x180);
  Display_Chinese2(0,140,ziku+0x1a0);
  Display_Chinese2(0,160,ziku+0x1b0);
  */
  for (int ti = 0;ti < 3;ti++){
      for(int tj = 0;tj < 12;tj++){
          Display_Chinese2((ti+1)*2,tj*16,ziku+(0x20*(ti*12+tj)));
      }
  }
    con_send('\n');

    return;

error:
    lprint("Err!\ndispcchar [x] [y]\n");
}
#endif

void buzztest(char *p)
{
    uint para = 1, tmp;

    tmp = get_howmany_para(p);
    if(tmp>=1){
	    p = str_to_hex(p, &para);
    }
    lprintf("para %x %d\n", para, para);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB,GPIO_Pin_5);	
  //led end

    while(!con_is_recved()){
	    /* Set*/
        if(para > 200)
            lprintf("Set PB5\n");
	    GPIO_ResetBits(GPIOB,GPIO_Pin_5);	
	    delay_ms(para);
	    /* ReSet */
        if(para > 200)
            lprintf("Reset PB5\n");
	    GPIO_SetBits(GPIOB,GPIO_Pin_5);	
	    delay_ms(para);
    }
    con_send('\n');

    return;

error:
    lprint("Err!\ndispcchar [x] [y]\n");
}
void ledtest(char *p)
{
    uint para = 0, tmp;

    tmp = get_howmany_para(p);
    if( tmp >= 1){
	    p = str_to_hex(p, &para);
    }
    if(para){
	    /* Set*/
	    GPIO_ResetBits(GPIOA,GPIO_Pin_12);	
	    GPIO_ResetBits(GPIOA,GPIO_Pin_2);	
	    lprintf("\n\rSet\n\r");
    }
    else{
	    /* ReSet PC13 */
	    GPIO_SetBits(GPIOA,GPIO_Pin_12);	
	    GPIO_SetBits(GPIOA,GPIO_Pin_2);	
	    lprintf("\n\rClr\n\r");
    }
    con_send('\n');

    return;

error:
    lprint("Err!\ndispcchar [x] [y]\n");
}
#if 0
void dispcchar(char *p)
{
    uint x = 0, y=0, offset = 0, tmp;

    tmp = get_howmany_para(p);
    if( tmp < 2)
        goto error;
    p = str_to_hex(p, &x);
    p = str_to_hex(p, &y);
    if(tmp > 2)
        str_to_hex(p, &offset);
    lprintf("x=%d y=%d\n", x, y);
    if(tmp == 2)
        Display_Chinese(x,y,jiong1);
    else
        Display_Chinese2(x,y,ziku+offset);
    con_send('\n');

    return;

error:
    lprint("Err!\ndispcchar [x] [y]\n");
}

void dispwb(char *p)
{
    uint page = 0, cl=0, data = 0, tmp;

    tmp = get_howmany_para(p);
    if( tmp < 3)
        goto error;
    p = str_to_hex(p, &page);
    p = str_to_hex(p, &cl);
    str_to_hex(p, &data);
    page = page%4;
    cl = cl % 192;
    data &= 0xff;
    lprintf("page=%x cl=%x d=%x\n", page, cl, data);
    lcd_write_byte(page, cl, data);
    con_send('\n');

    return;

error:
    lprint("Err!\ndwb [page] [col] [data]\n");
}
void Display_Chinese2_fullscreen(uint32_t x, uint32_t y, uint32_t zi_offset)
{
    for(int i = 0; i <48; i++)
    {
        Display_Chinese2(x, y, ziku + 0x20*zi_offset++);
        y += 16;
        if(y>191){
            y = 0;
            x+=2;
        }
    }
}

void show_ziku(char *p)
{
    uint stt_off = 0, stp_off =0, tmp, zikusize;
    uint zi, x=0, y=0;

    zikusize = get_ziku_size();
    stp_off = zikusize;
    lprintf("size of ziku:%x %d\n", stp_off, stp_off);
    tmp = get_howmany_para(p);
    if( tmp > 0)
        p = str_to_hex(p, &stt_off);
    if( tmp > 1)
        p = str_to_hex(p, &stp_off);
    if(stp_off > zikusize)
        stp_off = zikusize;
    zi = stt_off;
    while(zi<stp_off){
        lprintf("zi %d stop %d\n", zi, stp_off);
        Display_Chinese2_fullscreen(x,y,zi);
        delay_ms(1000);
        zi+=12;
        if(con_is_recved())
            break;
    }
    con_send('\n');

    return;
}
#endif
void lcdsuebstep(char *p)
{
    uint32_t para1 = 0, para2=0, para3 = 0, para4 = 0, color = 0,tmp, cmdindex;
    uint32_t para5 = 0, para6=0, para7 = 0, para8 = 0;

    lprintf("p=%s\n", p);
    tmp = get_howmany_para(p);
    lprintf("tmp=%d\n", tmp);
    if(tmp>=1)
	    p = str_to_hex(p, &cmdindex);
    if(tmp == 0 || cmdindex == 0){
	    lprintf("lcd basic_init\n");
	    lcd_sueb_basicinit();
    }
    else if(cmdindex == 1){//cs high/low
	lprintf("line %d read reg %x\n", __LINE__, LCD_RD_REG());
    }
    else if(cmdindex == 2){//
	lprintf("line %d read data %x\n", __LINE__, LCD_RD_DATA());
    }
    else if(cmdindex == 3){//
	if (tmp<2)goto error;

        str_to_hex(p, &para1);
	lprintf("para1 %x\n", para1);
	LCD_WR_REG(para1);
	lprintf("line %d wr reg para1 %x\n", __LINE__);
    }
    else if(cmdindex == 4){//
	if (tmp<2)goto error;

        str_to_hex(p, &para1);
	lprintf("para1 %x\n", para1);
	lprintf("line %d wr data para1 %x\n", __LINE__);
	LCD_WR_DATA(para1);
    }
    else if(cmdindex == 5){//cmd
	if (tmp<5)goto error;
        p=str_to_hex(p, &para1);
        p=str_to_hex(p, &para2);
        p=str_to_hex(p, &para3);
        p=str_to_hex(p, &para4);
	if(tmp==6)
        p=str_to_hex(p, &color);
        lcd_clr_window(color, para1, para2, para3, para4);
    }
    else if(cmdindex == 6){//cmd
	if (tmp<2)goto error;
        p=str_to_hex(p, &para1);
	lcd_sueb_init(para1);
    }
    else if(cmdindex == 7){
	if (tmp<7)goto error;
        p=str_to_hex(p, &para1);
        p=str_to_hex(p, &para2);
        p=str_to_hex(p, &para3);
        p=str_to_hex(p, &para4);
        p=str_to_hex(p, &para5);
        p=str_to_hex(p, &para6);
        p=str_to_hex(p, &para7);
	Show_Str(para1,para2,para3,para4,&para5,para6,para7);
    }
    else if(cmdindex == 8){//
	    main_test(); 		//测试主界面
    }
    else if(cmdindex == 9){//
	    Test_Color();  		//简单刷屏填充测试
    }
    else if(cmdindex == 0xa){//
	    Test_FillRec();		//GUI矩形绘图测试
    }
    else if(cmdindex == 0xb){//
	    lprintf("test english font.Any key continue...\n");
	    English_Font_test();//英文字体示例测试
    }
    else if(cmdindex == 0xc){//
	    lprintf("test trigangle drawing.Any key continue...\n");
	    Test_Triangle();    //GUI三角形绘图测试
    }
    else if(cmdindex == 0xd){//
	    lprintf("test chinese font.Any key continue...\n");
	    Chinese_Font_test();//中文字体示例测试
    }
    else if(cmdindex == 0xe){//
	    lprintf("test pic display.Any key continue...\n");
	    Pic_test();			//图片显示示例测试
    }
    else if(cmdindex == 0xf){//
	    lprintf("test touch.\n");
	    //如果不带触摸，或者不需要触摸功能，请注释掉下面触摸屏测试项
	    Touch_Test();		//触摸屏手写测试		
    }
    else if(cmdindex == 0x10){//
	    lprintf("test rotate display.Any key continue...\n");
	    Rotate_Test();   //旋转显示测试
    }
    else if(cmdindex == 0x11){//
	    lprintf("test circle drawing.Any key continue...\n");
	    Test_Circle(); 		//GUI画圆测试
    }
    else if(cmdindex == 0x12){//
    }
    else if(cmdindex == 0x13){//
    }
    else if(cmdindex == 0x14){//
    }
    else if(cmdindex == 0x15){//
    }
    else if(cmdindex == 0x16){//
    }
    con_send('\n');

    return;

error:
    lprint("Err!\ndispcchar [x] [y]\n");
}
void poweroff(char *p)
{
    lprintf("Power OFF!\n");
    GPIO_SetBits(GPIOB,GPIO_Pin_0);	
    con_send('\n');

    return;

}
void adc(char *p)
{
    con_send('\n');
    adc_test();

    return;

}
void cmd_exit(char *p)
{
    con_send('Quit CMD!\n');
    quit_cmd = 1;

    return;

}
void gpiotest(char *p)
{
    con_send('\n');
    gpio_test();

    return;

}
void rtc_cmd(char *p)
{
    uint8_t d[6], tmp, *ip=0;

    tmp = get_howmany_para(p);
    lprintf("tmp=%d\n", tmp);
    if(tmp==6){
        p = str_to_hex(p, &d[0]);
        p = str_to_hex(p, &d[1]);
        p = str_to_hex(p, &d[2]);
        p = str_to_hex(p, &d[3]);
        p = str_to_hex(p, &d[4]);
        p = str_to_hex(p, &d[5]);
        rtc_write(d);
    }
    else if(tmp==0){
        rtc_read(d);
    }
    else if(tmp==1){
        p = str_to_hex(p, &d[0]);
        lprintf("read reg %b=%b\n", d[0], rtc_read_reg(d[0]));
    }
    else if(tmp==2){
        p = str_to_hex(p, &d[0]);
        p = str_to_hex(p, &d[1]);
        lprintf("write reg %b->%b\n", d[0], d[1]);
        rtc_write_reg(d[0], d[1]);
    }
    else{
        lprintf("error \n");
    }
    con_send('\n');

    return;

}
static const struct command cmd_list[]=
{
    //{"dwb",dispwb},
    {"adc",adc},
    {"bz",buzztest},
    {"exit",cmd_exit},
    {"go",go},
    {"gpiotest",gpiotest},
    {"help",print_help},
    //{"lcd19264init",lcd19264init},
    //{"lcd19264dc",dispcchar},
    {"led",ledtest},
    {"lst",lcdsuebinit},
    {"lstep",lcdsuebstep},
    {"lstest",lcdsuebtest},
    {"pm",print_mem},
    {"poff",poweroff},
    {"r",read_mem},
    {"rtc",rtc_cmd},
    {"sd",sd},
    {"sdcmds",sd_cmds},
    //{"szk",show_ziku},
    {"w",write_mem},
    {NULL, NULL},
};
static uint32_t * mrw_addr;
void go(char *para)
{
    (void)para;
	(*((void (*)())mrw_addr))();
}

void print_help(char *para)
{
    (void)para;
    uint i = 0;
    lprintf("Version %s%s\n", VERSION, GIT_SHA1);
    lprint("Cmd:\n");
    while(1){
            if(cmd_list[i].cmd_name == NULL)
                    break;
	    lprint("--%s\n", cmd_list[i].cmd_name);
            i++;
    }
}

uint asc_to_hex(char c)
{
	if(c >= '0' && c <= '9')
		return c - '0';	
	if(c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	if(c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	return 0;
}

uint get_howmany_para(char *s)
{
	uint tmp = 0;
	while(1){
		while(*s == ' ')
			s++;
		if(*s)
			tmp++;
		while(*s != ' ' && *s)
			s++;
		if(!*s)
			return tmp;
	}
}

char * str_to_hex(char *s, uint * result)
{
	uint  i = 0;

	*result = 0;
	while(*s == ' ')s++;
	for(i=0;i<8;i++){
		if(*s == ' ' || *s == 0)
			break;
		*result = *result*16 + asc_to_hex(*s++);
	}
	while(*s == ' ')s++;
	return s;
}

void print_mem(char *p)
{
    uint length = 0x80, tmp, i;
    char *cp;

    tmp = get_howmany_para(p);
    if( tmp > 1)
        goto error;
    if(tmp == 0)
        goto print;
    str_to_hex(p, &length);
print:
    cp = (char *)mrw_addr;
    while(length){
	lprint("\n%x: ", (int)cp);
	for(i=0;i<16;i++){
		length--;
		lprintf("%b", *cp++);
		con_send(i == 7 ? '-':' ');
	}
    }
    con_send('\n');

    return;

error:
    lprint("Err!\npm [length](0x80 default)\n");

}

void write_mem(char *p)
{
    uint value, tmp;

    tmp = get_howmany_para(p);
    if(tmp == 0 || tmp > 2)
	goto error;
    p = str_to_hex(p, &value);
    if(tmp == 1)
        goto write;
    str_to_hex(p, (uint32_t*)&mrw_addr);
    mrw_addr = (uint32_t*)((uint32_t)mrw_addr & 0xfffffffc);
write:
    *(uint32_t*)mrw_addr = value;
    lprint("Write 0x%x@0x%x\n",value,mrw_addr);
    return;

error:
    lprint("Err!\nw (hexaddr) [(hexaddr)](last addr default)\n");

}


void read_mem(char *p)
{
    uint value, tmp;

    tmp = get_howmany_para(p);
    if( tmp > 1)
	goto error;
    if(tmp == 0)
    	goto read;
    str_to_hex(p, (uint32_t*)&mrw_addr);
    mrw_addr = (uint32_t*)((uint32_t)mrw_addr & 0xfffffffc);
read:
    value = *(uint32_t*)mrw_addr;
    lprint("Read 0x%x at memory 0x%x\n",value,mrw_addr);

    return;

error:
    lprint("Err!\nr [(hexaddr)](last addr default)\n");

}

void lmemset(char *d,unsigned char v,unsigned int n)
{
	while(n--)*d++=v;
}

void handle_cmd()
{
    unsigned char i = 0;
    char *p_cmd, *p_buf;

    lprint("\n");
    if(!cmd_buf[0])
	return;
    while(1){
	    if(cmd_list[i].cmd_name == NULL)
		    break;
	    p_cmd=cmd_list[i].cmd_name;
	    p_buf=cmd_buf;
	    while(*p_cmd){
		    if(*p_buf != *p_cmd)
			    break;
		    p_buf++;
		    p_cmd++;
	    }
	    if(!(*p_cmd) && (*p_buf == ' ' || !(*p_buf))){
            	    cmd_list[i].cmd_fun(p_buf);
                    lprint("'%s' done.\n", cmd_list[i].cmd_name);
            	    return;
       	    }
	    i++;
    }
    lprint("Unknow cmd:%s\n%s",cmd_buf, "Please check the cmd list.");
}

uint time_limit_recv_byte(uint limit, char * c);
void run_cmd_interface()
{
	char c;
	int timeout = 3;
	
	mrw_addr = (uint32_t*)0x20000000;
	lprintf("Version %s%s\n", VERSION, GIT_SHA1);
	lprint("\n\nclean_cmd. \n'c' key go cmd...\n");
	while(timeout--){
		delay_ms(1000);
		if(con_is_recved() && (con_recv() == 'c'))break;
		lprintf("timeout %d\n", timeout);
		if(timeout == 1){
			lprintf("Timeout. Quit cmd\n");
			return;
		}
	}
	lmemset(cmd_buf, 0, COM_MAX_LEN);
	cmd_buf_p = 0;
	lprint("\nCleanCMD>");
	
	while(!quit_cmd){
		c = con_recv();
		if(c == ENTER_CHAR || c == 0x1b || c== 0x03){
			if(c == ENTER_CHAR)
				handle_cmd();
			lmemset(cmd_buf, 0, COM_MAX_LEN);
			cmd_buf_p = 0;
			lprint("\nCleanCMD>");
		}
		else if(c == 0x08){
			if(!cmd_buf_p)
				continue;
                        cmd_buf[--cmd_buf_p] = 0;
                        lprintf("\b \b");
		}
		else{
			if(cmd_buf_p < (COM_MAX_LEN - 1)){
				cmd_buf[cmd_buf_p++] = c;
				con_send(c);
			}
		}
	}
    quit_cmd = 1;
}
