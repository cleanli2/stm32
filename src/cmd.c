#include "fs.h"
#include "cmd.h"
#include "common.h"
#include "lprintf.h"
#include "sd/stm32_eval_spi_sd.h"
#include <stdint.h>
#include <string.h>
#include <display.h>
#define uint uint32_t
#define lprint lprintf

static char cmd_buf[COM_MAX_LEN] = "";
static uint cmd_buf_p = COM_MAX_LEN;
static uint quit_cmd = 0;
extern uint32_t task_mask;
extern uint32_t logv;

uint get_howmany_para(char *s);
char * str_to_str(char *s, char**result);
/*
void con_send(char X)
{
    __io_putchar(X);
    if(X == '\n'){
        __io_putchar('\r');
    }
}
*/
uint32_t ci=0;

uint8_t cmd_caches[CMD_CACHES_SIZE][COM_MAX_LEN] = {0};
uint32_t cmdcache_index=0;
uint32_t review_cmd_his_index;
uint8_t read_buf[512];

extern const uint8_t ziku12[];
extern const uint8_t ziku[];
uint32_t get_ziku12_size();
uint32_t get_ziku_size();
void save_sd_log()
{
    lprintf("save log to sd card:stmlog.txt\n");
    u32 ch_cnt = 0, i512_ct=0, show_ct=0;
    u32 addr = SPI_FLASH_LOG_START;
    if(FS_OK!=open_file_for_write("STMLOG", "TXT")){
        lprintf_time("open file fail:stmlog.txt\n");
        return;
    }
    memset(read_buf, ' ', 512);
    lprintf("run task log\n");
    task_log(NULL);
    dt_us_last();
    while(1){
        SPI_Flash_Read(read_buf,addr,512);
        for(i512_ct=0;i512_ct<512;i512_ct++){
            if(0xff==read_buf[i512_ct]){
                read_buf[i512_ct]='`';
            }
            else if('\r'==read_buf[i512_ct]||
                    '\n'==read_buf[i512_ct]){
            }
            else if(0x20>read_buf[i512_ct]){
                read_buf[i512_ct]='?';
            }
        }
        ch_cnt+=512;
        show_ct+=512;
        if(show_ct>(SPI_FLASH_LOG_SIZE/10)){
            lprintf("%d%%doing\n", ch_cnt*100/SPI_FLASH_LOG_SIZE);
            show_ct=0;
        }
        if(ch_cnt >= SPI_FLASH_LOG_SIZE){
            write_sec_to_file((const char*)read_buf);
            close_file();
            u32 tmus=dt_us_last();
            lprintf_time("save log OK:%dk %dus %dk/s\n",
                    SPI_FLASH_LOG_SIZE/1000,
                    tmus,
                    SPI_FLASH_LOG_SIZE*1000/tmus);
            return;
        }
        write_sec_to_file((const char*)read_buf);
        addr+=512;
    }
}
void w25f(char *p)
{
    uint32_t para1 = 0, para2=0, para3 = 0, tmp, cmdindex;
#ifdef WRITE_W25F
    const uint8_t*datawritebuf=ziku;
    uint32_t datalen = get_ziku_size();
#endif

    lprintf("p=%s\n", p);
    tmp = get_howmany_para(p);
    lprintf("tmp=%d\n", tmp);
    if(tmp>=1){
	    p = str_to_hex(p, &cmdindex);
    }
    if(tmp == 0 || cmdindex == 0){
        SD_LowLevel_Init();
        lprintf("w25 flash ID:%x\n", SPI_Flash_ReadID());
    }
    else if(cmdindex == 1){
        if(tmp>1){
            p = str_to_hex(p, &para1);
        }
        SPI_Flash_Read((uint8_t*)&para3, para1, 4);
        lprintf("w25 read 4 bytes = %X @%x\n", para3, para1);
    }
    else if(cmdindex == 2){//
        lprintf("w25f 2 addr data\n");
        if(tmp<3){
            goto err;
        }
        p = str_to_hex(p, &para1);//addr
        p = str_to_hex(p, &para2);//data
        SPI_Flash_Write((uint8_t*)&para2, para1, 4);
        lprintf("w25 write 4 bytes = %X @%x\n", para2, para1);
    }
    else if(cmdindex == 3){//
        if(tmp>1){
            p = str_to_hex(p, &para1);
        }
        para2 = 1;
        if(tmp>2){
            p = str_to_hex(p, &para2);
        }
        while(para2--){
            SPI_Flash_Read((uint8_t*)read_buf, para1, 512);
            mem_print((const char*)read_buf, para1, 512);
            para1+=512;
            lprintf("\n");
        }
    }
    else if(cmdindex == 4){//
        lprintf("erase all chip.\n");
        if(tmp<2){
            goto err;
        }
        p = str_to_hex(p, &para1);
        if(para1 != 0xe4a5ea11){
            lprintf("erase fail!");
        }
        else{
            SPI_Flash_Erase_Chip();
        }
    }
    else if(cmdindex == 5){//fake w w25f
#ifdef WRITE_W25F
        if(tmp>1){
            p = str_to_hex(p, &para1);
            lprintf("para1=%x, wbuf=0x%x, datalen=%d 0x%x\n", para1,
                    datawritebuf, datalen, datalen);
            //SPI_Flash_Write((uint8_t*)para1, datawritebuf, datalen);
        }
        else{
            lprintf("error para:w25f 5 70000//write to 0x70000");
        }
#endif
    }
    else if(cmdindex == 6){//cmd
        p = str_to_hex(p, &para1);
        p = str_to_hex(p, &para2);
        p = str_to_hex(p, &para3);
    }
    else if(cmdindex == 7){//w w25f
#ifdef WRITE_W25F
        if(tmp>2){
            p = str_to_hex(p, &para1);
            p = str_to_hex(p, &para2);
            lprintf("para1=%x, wbuf=0x%x, para2=%d\n", para1,
                    datawritebuf, datalen);
            SPI_Flash_Write(datawritebuf, para1, para2);
        }
        else{
            lprintf("error para:w25f 5 70000 18000//write to 0x70000, len 0x18000");
        }
#endif
    }
    else if(cmdindex == 8){//
        lprintf("erase one sector.\n");
        if(tmp<2){
            goto err;
        }
        p = str_to_hex(p, &para1);
        p = str_to_hex(p, &para2);
        if(para2 != 0xe4a5e07e){
            lprintf("erase %X fail", para1);
        }
        else{
            SPI_Flash_Erase_Sector(GET_SECTOR_ADDR(para1));
        }
    }
    else if(cmdindex == 0x9){//
        lprintf("switch env area\n");
        switch_env_area();
    }
    else if(cmdindex == 0xa){//
        lprintf("switch env area with data\n");
        p = str_to_hex(p, &para1);
        if(para1 != 0x5e14c6e7){
            lprintf("switch env fail", para1);
            goto err;
        }
        switch_env_area_with_data();
    }
    else if(cmdindex == 0xb){//
        lprintf("erase cur env area\n");
        p = str_to_hex(p, &para1);
        if(para1 != 0xe4a5ee7c){
            lprintf("erase env fail", para1);
        }
        else{
            erase_env_area();
        }
    }
    else if(cmdindex == 0xc){//
        lprintf("print log in spi flash:\n");
        spi_flash_log_print();
    }
    else if(cmdindex == 0xd){//
        lprintf("run task log\n");
        task_log(NULL);
    }
    else if(cmdindex == 0xe){//
        save_sd_log();
    }
    con_send('\n');

    return;
err:
    lprintf("ERROR para\n");
}

//u8 SD_Initialize(void);
#if 1
void sd(char *p)
{
    uint32_t para1 = 0, para2=0, para3 = 0, tmp, cmdindex;
    SD_Error Status = SD_OK;
    SD_CardInfo mycard;

    lprintf("p=%s\n", p);
    //strcpy(cmd_caches[ci++], p);
    if(ci > 40)
	    ci = 0;
    tmp = get_howmany_para(p);
    lprintf("tmp=%d\n", tmp);
    if(tmp>=1)
	    p = str_to_hex(p, &cmdindex);
    lprintf("cmdind=%d\n", cmdindex);
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
        lprintf("read sd block:sd a addr repeat\n");
        para1=0;
        para2=1;
        if(tmp>1){
            p = str_to_hex(p, &para1);
        }
        if(tmp>2){
            p = str_to_hex(p, &para2);
        }
        lprintf("addr %x\n", para1);
        lprintf("repeat %x\n", para2);
        while(para2--){
            SD_ReadBlock(read_buf, para1, 512);
            mem_print((const char*)read_buf, para1*512, 512);
            para1 += 1;
        }
    }
    else if(cmdindex == 0xb){//
        sf_read_id();
    }
    else if(cmdindex == 0xc){//
        str_to_hex(p, &para1);
        lprintf("spi choose %x\n", para1);
        spi_choose_stm32(para1);
    }
    else if(cmdindex == 0xd){//
        str_to_hex(p, &para1);
        lprintf("spi speed %x\n", para1);
        spi_speed(para1);
    }
    else if(cmdindex == 0xe){//
        lprintf("write sd block:sd e filldata blockaddr repeat\n");
        para1=0x55;
        para2=0x0;
        para3=1;
        if(tmp>1){
            p = str_to_hex(p, &para1);
        }
        if(tmp>2){
            p = str_to_hex(p, &para2);
        }
        if(tmp>3){
            p = str_to_hex(p, &para3);
        }
        lprintf("fill %x\n", para1);
        lprintf("blockaddr %x\n", para2);
        lprintf("dataaddr %W\n", ((uint64_t)para2)<<9);
        memset(read_buf, para1, 512);
        while(para3--){
            lprintf("write @%Wsd block ret:%x\n", ((uint64_t)para2)<<9,
                    SD_WriteBlock(read_buf, ((uint64_t)para2), 512));
            para2++;
        }
    }
    else if(cmdindex == 0xf){//
        //SD_Initialize();
    }
    con_send('\n');

    return;

//error:
//    lprint("Err!\ndispcchar [x] [y]\n");
}
#if 0
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

//error:
//    lprint("Err!\ndispcchar [x] [y]\n");
}

void lcdsuebtest(char *p)
{
    (void)p;
    lcd_sueb_test();
    con_send('\n');

    return;

//error:
//    lprint("Err!\ndispcchar [x] [y]\n");
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
    RCC_APB2PeriphClockCmd(BEEP_GPIO_PERIPH, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(BEEP_GPIO_GROUP, &GPIO_InitStructure);
    GPIO_ResetBits(BEEP_GPIO_GROUP, BEEP_GPIO_PIN);
  //led end

    while(!con_is_recved()){
	    /* Set*/
        if(para > 200){
            lprintf("Set PB5\n");
        }
	    GPIO_ResetBits(BEEP_GPIO_GROUP, BEEP_GPIO_PIN);
	    delay_ms(para);
	    /* ReSet */
        if(para > 200){
            lprintf("Reset PB5\n");
        }
	    GPIO_SetBits(BEEP_GPIO_GROUP, BEEP_GPIO_PIN);
	    delay_ms(para);
    }
    con_send('\n');

    return;

//error:
//    lprint("Err!\ndispcchar [x] [y]\n");
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

//error:
//    lprint("Err!\ndispcchar [x] [y]\n");
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
//void LCD_BUS_To_write(int write);
void bus_to_lcd(int mode_to_lcd);
void lcdsuebstep(char *p)
{
    uint32_t para1 = 0, para2=0, para3 = 0, para4 = 0, color = 0,tmp, cmdindex;
    uint32_t para5 = 0, para6=0, para7 = 0;

    //LCD_BUS_To_write(1);
    bus_to_lcd(1);
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
	Show_Str(para1,para2,para3,para4,(char*)&para5,para6,para7, 1);
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
        if (tmp<2)goto error;
        p=str_to_hex(p, &para1);
        para1&=0xffff;
        lprintf("para1 %x\n", para1);
        set_BL_value(para1);
    }
    else if(cmdindex == 0x13){//
        set_touch_need_reinit();
    }
    else if(cmdindex == 0x14){//
        para1 = 100;
        if(tmp>0){
            p=str_to_hex(p, &para1);
        }
        while(para1--){
            uint16_t touch_x, touch_y;
            lprintf("ct%d\r", para1);
            if(get_TP_point(&touch_x, &touch_y)){
                lprintf("txy=%d,%d\n", (uint32_t)touch_x, (uint32_t)touch_y);
            }
            delay_ms(100);
        }
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
    (void)p;
    lprintf("Power OFF!\n");
    power_off();
    con_send('\n');

    return;

}
void adc(char *p)
{
    (void)p;
    con_send('\n');
    adc_test();

    return;

}
void cmd_exit(char *p)
{
    (void)p;
    lprintf("Quit CMD!\n");
    quit_cmd = 1;

    return;

}
void rebootd(char *p)
{
    (void)p;

    reboot_download();

    return;

}
void reboot(char *p)
{
    (void)p;
    soft_reset_system();

    return;

}
#define BLOCK_SIZE 0x400
void fmerase(char *p)
{
    uint32_t er_ct = 1, addr;
    //uint32_t len = 0xffff, data = 0;
    //uint32_t dp = (uint32_t*)0x08010000;
    FLASH_Status fret;

    lprintf("\n");
    uint32_t tmp = get_howmany_para(p);
    if(tmp==0)goto err;
    if(tmp>=1){
        p = str_to_hex(p, &addr);
    }
    if(tmp>=2){
        p = str_to_hex(p, &er_ct);
    }
    FLASH_Unlock();
    while(er_ct--){
        if(FLASH_COMPLETE == (fret=FLASH_ErasePage(addr))){
            lprintf("flash oper done @%X\n", addr);
        }
        else{
            lprintf("flash oper err:%d @addr %X\n", fret, addr);
        }
        addr+=BLOCK_SIZE;
    }
    FLASH_Lock();
#if 0
    while(len){
        if(*dp++ != data){
            ne_ct++;
            lprintf("\rNE:%d", ne_ct);
        }
        len -= 4;
    }
#endif

    return;
err:
    lprintf("err:fmerase addr count\n");
    return;

}
void fmwtest(char *p)
{
    uint32_t len = 1, data = 0, addr;
    //uint32_t ne_ct = 0;
    //uint32_t dp = (uint32_t*)0x08010000;
    FLASH_Status fret;

    lprintf("\n");
    uint32_t tmp = get_howmany_para(p);
    if(tmp>=2){
        p = str_to_hex(p, &data);
        p = str_to_hex(p, &addr);
        if(tmp>=3) p = str_to_hex(p, &len);
        FLASH_Unlock();
        while(len){
            if(FLASH_COMPLETE == (fret=FLASH_ProgramWord(addr, data))){
                //lprintf("flash oper done\n");
            }
            else{
                lprintf("flash oper err:%d adr %X\n", fret, addr);
            }
            addr+=4;
            len-=4;
        }
        FLASH_Lock();
    }
    else{
        lprintf("err:fmwtest data addr (len)\n");
    }
#if 0
    while(len){
        if(*dp++ != data){
            ne_ct++;
            lprintf("\rNE:%d", ne_ct);
        }
        len -= 4;
    }
#endif

    return;

}
void fmrtest(char *p)
{
    uint32_t len = 0xffff, ne_ct = 0, data = 0, addr;
    uint32_t* dp = (uint32_t*)0x08010000;

    lprintf("\n");
    uint32_t tmp = get_howmany_para(p);
    if(tmp<3){
        goto err;
    }
    else{
        p = str_to_hex(p, &data);
        p = str_to_hex(p, &addr);
        p = str_to_hex(p, &len);
        dp = (uint32_t*)addr;
        while(len){
            if(*dp++ != data){
                ne_ct++;
                lprintf("\rNE:%X addr %X", ne_ct, (uint32_t)(dp-1));
            }
            len -= 4;
        }
    }
    lprintf("\n\n");
    return;

err:
    lprintf("fmrtest data addr len\n");
    return;

}
void usb_strg_init(char *p)
{
    uint32_t n, p1 = 0;
    n = get_howmany_para(p);
    if(n>0){
        p = str_to_hex(p, &p1);
        lprintf("p1=%d\n", p1);
    }
    usb_main_init(p1);
}
void logflag(char *p)
{
    uint32_t n, p1 = 0;
    n = get_howmany_para(p);
    if(n>0){
        p = str_to_hex(p, &p1);
        lprintf("logf set to %x\n", p1);
    }
    logv = p1;
}
void test(char *p)
{
    u32 p1,p2;
    p = str_to_hex(p, &p1);
    p = str_to_hex(p, &p2);
    //os_lock_test();
    //put_sound(p1, p2);
#if 0
    uint32_t px1, py1, s,e;
    static int txc=0;
    p = str_to_hex(p, &px1);
    p = str_to_hex(p, &py1);
    p = str_to_hex(p, &s);
    p = str_to_hex(p, &e);
    lprintf("para:%x %x %x %x\n", px1,py1,s,e);

    LCD_Clear(WHITE);
    if(txc++&0x1){
        lprintf("old\n");
        LCD_DrawLine(240, 400, px1, py1);
    }
    else{
        lprintf("new\n");
        LCD_DrawLine_direction(240, 400, px1, py1, s, e);
    }
#endif
#if 0
    uint32_t n, p1, p2;
    uint64_t t1, t2, t3,t4,t5,t6,t7;
    t1= get_system_us();
    n = get_howmany_para(p);
    t2= get_system_us();
    p = str_to_hex(p, &p1);
    beep_by_timer(p1);
    return;
    t3= get_system_us();
    if(n==1){
    t4= get_system_us();
        delay_us(p1);
    t5= get_system_us();
    }
    else{
    t4= get_system_us();
        delay_ms(p1);
    t5= get_system_us();
    }
    t6= get_system_us();
    t7= get_system_us();
    lprintf("1:%U\n", t1);
    lprintf("2:%U\n", t2);
    lprintf("3:%U\n", t3);
    lprintf("4:%U\n", t4);
    lprintf("5:%U\n", t5);
    lprintf("6:%U\n", t6);
    lprintf("7:%U\n", t7);
#endif
    return;

}
void tm(char *p)
{
    uint32_t tmp, para;
    con_send('\n');
    tmp = get_howmany_para(p);
    lprintf("tmp=%d\n", tmp);
    if(tmp<1){
        return;
    }
    p = str_to_hex(p, &para);
    task_mask = para;
    lprintf("task_mask=%X\n", task_mask);

    return;

}

void history(char *p)
{
    (void)p;
    uint32_t n = CMD_CACHES_SIZE, cix = cmdcache_index;
    con_send('\n');
    while(n--){
        if(cix==0){
            cix = CMD_CACHES_SIZE-1;
        }
        else{
            cix--;
        }
        if(cmd_caches[cix][0]!=0){
            lprintf("%s\n", cmd_caches[cix]);
        }
    }
    con_send('\n');

    return;
}

void keytest(char *p)
{
    (void)p;
    uint8_t rchar;
    con_send('\n');
    while((rchar=con_recv())!=ENTER_CHAR){
        lprintf("%b\n", rchar);
    }
    con_send('\n');

    return;

}
void envprint(char *p)
{
    int tmp = get_howmany_para(p);
    lprintf("number of para %d\n", tmp);
    if(tmp>0){
        lprintf("raw env:\n");
        printrawenv();
    }
    else{
        printenv();
    }
    con_send('\n');

    return;

}
void envget(char *p)
{
    uint32_t tmp;
    char* name, value[ENV_MAX_VALUE_LEN];
    tmp = get_howmany_para(p);
    lprintf("tmp=%d\n", tmp);
    if(tmp<1){
        lprintf("err\n");
        return;
    }
    str_to_str(p, &name);
    if(ENV_FAIL == get_env(name, value)){
        lprintf("get_env fail\n");
        return;
    }
    else{
        lprintf("get_env OK\n");
    }
    lprintf("%s=%s\n", name, value);
    con_send('\n');

    return;

}
void envset(char *p)
{
    uint32_t tmp;
    char* name, *value=0;
    tmp = get_howmany_para(p);
    lprintf("tmp=%d\n", tmp);
    if(tmp<1){
        lprintf("err\n");
        return;
    }
    if(tmp>=1){
        p = str_to_str(p, &name);
    }
    if(tmp>=2){
        p = str_to_str(p, &value);
    }
    if(value){
        lprintf("todo: %s=%s\n", name, value);
    }
    else{
        lprintf("todo: del %s\n", name);
    }
    if(ENV_FAIL == set_env(name, value)){
        lprintf("set_env fail\n");
        return;
    }
    else{
        lprintf("set_env OK\n");
    }
    con_send('\n');

    return;

}
void rtcs(char *p)
{
    (void)p;
    con_send('\n');
    if(RTC_OK==adjust_1min(0)){
        lprintf("slower 1 min OK\n");
    }
    else{
        lprintf("slower 1 min FAIL\n");
    }
    return;
}
void rtcf(char *p)
{
    (void)p;
    con_send('\n');
    if(RTC_OK==adjust_1min(1)){
        lprintf("faster 1 min OK\n");
    }
    else{
        lprintf("faster 1 min FAIL\n");
    }
    return;
}
void gpiotest(char *p)
{
    (void)p;
    con_send('\n');
    gpio_test();

    return;

}
void ps(char *p)
{
    (void)p;
    lprintf("ps start:\n");
  if(os_is_running){
    while(!con_is_recved()){
        lprintf("===============\n");
        showtasks();
        os_10ms_delay(1000);
    }
  }
  else{
    showtasks();
  }
    return;
}
void m_p(char *p)
{
    (void)p;
    uint32_t tmp, i=0;
    tmp = get_howmany_para(p);
    if(tmp>0){
        p = str_to_hex(p, &i);
    }
    play_music(music_lst[i], 0);
    return;
}
struct point stress_pts[] = {
    {225, 305}
};
void stress(char *p)
{
    (void)p;
    u32 para_n, test_time, intv;
    para_n = get_howmany_para(p);
    if(para_n<=1){
        lprintf("please input test time minutes & touch interval\nstress min interval\n");
        return;
    }
    p = str_to_hex(p, &test_time);
    p = str_to_hex(p, &intv);
    gftp->pts = NULL;//disable currentt test
    gftp->n_pt = 1;
    gftp->cur_n_pt = 0;
    gftp->interval = intv;
    gftp->cur_interval = 0;
    gftp->last= test_time;
    gftp->start= g_ms_count;
    lprintf("##########stress test start %s\n", get_rtc_time(0));
    lprintf("##########stress touch interval 20*%d ms\n", gftp->interval);
    lprintf("##########stress test time long %d mins\n", gftp->last);
    gftp->pts = stress_pts;
    return;
}
extern int debug_fs;
extern int disk_retry;
char file_name[16];
u32 fnn=0;
extern char debug_log_buf[DEBUG_LOG_BUF_SIZE+1];
int cam_dump_lines(u32 l);
void check_lines();
void cam_init(int);
void cam_al422(const char*ps, uint32_t p2);
void cam_deinit();
void cam_read_frame(int);
void cam_read_line(int);
void set_xclk(uint32_t fct);
void cam_set_rfn(u32 irn, u32 ifn);
uint8_t cam_r_reg(uint8_t addr);
void cam_save_1_frame(u32 only_uart_dump);
int cam_w_reg(uint8_t addr, uint8_t data);
void cam_to_lcd_1_frame();
void file_to_lcd();
void cam(char *p)
{
    SD_Error Status = SD_OK;
    SD_CardInfo mycard;
    char*p1, *ps;
    uint32_t np, p2=0x53, p3=3;
    bus_to_lcd(0);
    if(!file_name[0])strcpy(file_name, "YUV2");
    lprintf("filename:%s\n", file_name);
    np = get_howmany_para(p);
    lprintf("number of para=%d\n", np);
    if(np>=1){
        p = str_to_str(p, &p1);
        lprintf("p1=%s\n", p1);
    }
    if(!strcmp(p1, "sdp")){
        //PC0 is used for sd power control
        if(np>=2){
            p = str_to_hex(p, &p2);
            lprintf("p2=%d\n", p2);
            if(!p2){
                SD_power_on();
            }
            else{
                SD_power_off();
            }
        }
        else lprintf("need para\n");
    }
    if(!strcmp(p1, "sd")){
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
    }
    if(!strcmp(p1, "fwrite")){
        lprintf("memset %x\n", p2);
        if(np>=2){
            p = str_to_hex(p, &p2);
            lprintf("p2=%d\n", p2);
        }
        if(np>=3){
            p = str_to_hex(p, &p3);
            lprintf("p3=%d\n", p3);
        }
        if(FS_OK==open_file_for_write(file_name, "BIN")){
            uint32_t write_secs=0;
            while(p3--){
                memset((char*)read_buf, p2++, 512);
                if(0>write_sec_to_file((const char*)read_buf)){
                    break;
                }
                write_secs++;
            }
            lprintf("write_secs=%d\n", write_secs);
            close_file();
        }
        else{
            lprintf("open file fail\n");
        }
    }
    if(!strcmp(p1, "fread")){
        if(np>=2){
            p = str_to_hex(p, &p2);
            lprintf("p2=%d\n", p2);
        }
        if(np>=3){
            p = str_to_hex(p, &p3);
            lprintf("p3=%d\n", p3);
        }
        lprintf("file offset %d\n", p2);
        if(get_file_content((char*)read_buf, file_name, "BIN", p2, 512)>=0){
            mem_print((const char*)read_buf, (uint32_t)p3, 512);
        }
        else{
            lprintf("read file fail\n");
        }
        close_file();
    }
    if(!strcmp(p1, "nn")){
        if(np>=2){
            p = str_to_hex(p, &p2);
            lprintf("p2=%d\n", p2);
            fnn=p2;
        }
        slprintf(file_name, "YUV%x", fnn);
        lprintf("current is %s.BIN\n", file_name);
    }
    if(!strcmp(p1, "name")){
        if(np>=2){
            p = str_to_str(p, &ps);
            lprintf("ps=%s\n", ps);
            strcpy(file_name, ps);
        }
    }
    if(!strcmp(p1, "dbg")){
        if(np>=2){
            p = str_to_hex(p, &p2);
            lprintf("p2=%d\n", p2);
            debug_fs = p2;
        }
    }
    if(!strcmp(p1, "retry")){
        if(np>=2){
            p = str_to_hex(p, &p2);
            lprintf("p2=%d\n", p2);
            disk_retry = p2;
        }
        else{
            lprintf("disk retry=%d\n", disk_retry);
        }
    }
    if(!strcmp(p1, "camw")){
        p2 = 0xffffffff;
        lprintf("open %s.BIN\n", file_name);
        if(np>=2){
            p = str_to_hex(p, &p2);
            lprintf("p2=%d\n", p2);
        }
        if(p2 == 0xffffffff){//write to file
            if(FS_OK==open_file_for_write(file_name, "BIN")){
                //cam_read_frame((int)p2);
                close_file();
            }
            else{
                lprintf("open file fail\n");
            }
        }
        else{
            //cam_read_frame((int)p2);
        }
    }
    if(!strcmp(p1, "xclk")){
        p2 = 2;
        if(np>=2){
            p = str_to_hex(p, &p2);
        }
        lprintf("p2=%d\n", p2);
        set_xclk(p2);
    }
    if(!strcmp(p1, "fs")){
        if(np>=2){
            lprintf("just dump frame with uart\n");
            cam_save_1_frame(1);
        }
        else{
            lprintf("save frame to file %s.bin\n", file_name);
            if(FS_OK==open_file_for_write(file_name, "BIN")){
                cam_save_1_frame(0);
                close_file();
                slprintf(file_name, "YUV%x", ++fnn);
                lprintf("current is %s.BIN\n", file_name);
            }
            else{
                lprintf("open file fail\n");
            }
        }
    }
    if(!strcmp(p1, "fsc")){
        while(1){
            slprintf(file_name, "YUV%d", ++fnn);
            lprintf("save frame to file %s.bin\n", file_name);
            if(FS_OK==open_file_for_write(file_name, "BIN")){
                cam_save_1_frame(0);
                close_file();
                lprintf("\n===============save frame to file %s.bin done\n", file_name);
            }
            else{
                lprintf("open file fail:%s.BIN\n", file_name);
                lprintf("Stop fsc cmd\n");
                return;
            }
        }
    }
    if(!strcmp(p1, "w")){
        p2 = 0xffffffff;
        lprintf("open %s.BIN\n", file_name);
        if(np>=2){
            p = str_to_hex(p, &p2);
            lprintf("p2=%d\n", p2);
        }
        if(p2 == 0xffffffff){//write to file
            if(FS_OK==open_file_for_write(file_name, "BIN")){
                cam_read_line((int)p2);
                close_file();
            }
            else{
                lprintf("open file fail\n");
            }
        }
        else{
            cam_read_line((int)p2);
        }
    }
    if(!strcmp(p1, "srfn")){
        p2 = 2;
        p3 = 2;
        if(np>=2){
            p = str_to_hex(p, &p2);
        }
        if(np>=3){
            p = str_to_hex(p, &p3);
        }
        lprintf("p2=%d\n", p2);
        lprintf("p3=%d\n", p3);
        cam_set_rfn(p2, p3);
    }
    if(!strcmp(p1, "off")){
        lprintf("camera power down\n");
        cam_deinit();
        lprintf_time("camera power down\n");
    }
    if(!strcmp(p1, "init")){
        p2 = 0;
        if(np>=2){
            p = str_to_hex(p, &p2);
        }
        lprintf("p2=%d\n", p2);
        cam_init(p2);
    }
    if(!strcmp(p1, "lcd")){
        cam_to_lcd_1_frame();
    }
    if(!strcmp(p1, "sdlcd")){
        if(FS_OK==open_file_for_write("SDLCD", "BIN")){
            cam_save_1_frame(0);
            close_file();
            lprintf("\n===============save frame to file SDLCD.bin done\n");
            open_file_r("SDLCD.BIN");
            file_to_lcd();
            close_file();
            lprintf("\n===============file SDLCD.bin to lcd done\n");
        }
        else{
            lprintf("open file fail:SDLCD.BIN\n");
            return;
        }
    }
    if(!strcmp(p1, "lines")){
        check_lines();
    }
    if(!strcmp(p1, "dl")){
        p2 = 0;
        if(np>=2){
            p = str_to_hex(p, &p2);
        }
        lprintf("p2=%d\n", p2);
        cam_dump_lines(p2);
    }
    if(!strcmp(p1, "al422")){
        p2 = 0;
        if(np<3){
            lprintf("err, should 3 paras\n");
            goto err;
        }
        p = str_to_str(p, &ps);
        p = str_to_hex(p, &p2);
        lprintf("ps=%s\n", ps);
        lprintf("p2=%d\n", p2);
        cam_al422(ps, p2);
    }
    if(!strcmp(p1, "i2c")){
        if(np>=2){
            p = str_to_hex(p, &p2);
            lprintf("p2=%d\n", p2);
        }
        if(np>=3){
            p = str_to_hex(p, &p3);
            lprintf("p3=%d\n", p3);
            lprintf("cam w %b->%b return %x\n", p3, p2, cam_w_reg(p2, p3));
        }
        else{
            if(p2<0x100)
                lprintf("cam read %b=%b\n", p2, cam_r_reg(p2));
            else{
                lprintf("start of cam i2c dump----------------\n");
                for(uint16_t i=0;i<=0xff;i++){
                    lprintf("cam_w_reg(0x%b, 0x%b)\n",  i, cam_r_reg(i));
                }
                lprintf("end of cam i2c dump----------------\n");
            }
        }
    }
err:
    return;
}

void bflog(char *p)
{
    (void)p;
    debug_log_buf[DEBUG_LOG_BUF_SIZE]=0;
    putchars(debug_log_buf);
    putchars("\n");
    return;
}
void rtc_cmd(char *p)
{
    uint8_t d[6], tmp;

    tmp = get_howmany_para(p);
    lprintf("tmp=%d\n", tmp);
    if(tmp==6){
        p = str_to_hex(p, (uint*)&d[0]);
        p = str_to_hex(p, (uint*)&d[1]);
        p = str_to_hex(p, (uint*)&d[2]);
        p = str_to_hex(p, (uint*)&d[3]);
        p = str_to_hex(p, (uint*)&d[4]);
        p = str_to_hex(p, (uint*)&d[5]);
        rtc_write(d);
        set_env("LastTimeAdj", get_rtc_time(NULL));
    }
    else if(tmp==0){
        lprintf("%s\n", get_rtc_time(0));
    }
    else if(tmp==1){
        p = str_to_hex(p, (uint*)&d[0]);
        lprintf("read reg %b=%b\n", d[0], rtc_read_reg(d[0]));
    }
    else if(tmp==2){
        p = str_to_hex(p, (uint*)&d[0]);
        p = str_to_hex(p, (uint*)&d[1]);
        lprintf("write reg %b->%b\n", d[0], d[1]);
        rtc_write_reg(d[0], d[1]);
    }
    else{
        lprintf("error \n");
    }
    con_send('\n');

    return;

}
void file_op(char *p)
{
    uint32_t tmp;
    char* path;
    tmp = get_howmany_para(p);
    lprintf("tmp=%d\n", tmp);
    if(tmp<1){
        lprintf("err\n");
        return;
    }
    if(tmp>=1){
        p = str_to_str(p, &path);
        if(FS_OK==open_file_w(path)){
            lprintf("Open %s OK\n", path);
        }
        else{
            lprintf("Open %s fail\n", path);
        }
        close_file();
    }
    con_send('\n');

    return;
}
static const struct command cmd_list[]=
{
    //{"dwb",dispwb},
    {"adc",adc},
    {"bz",buzztest},
    {"bflog",bflog},
    {"cam",cam},
    {"exit",cmd_exit},
    {"envset",envset},
    {"envget",envget},
    {"envprint",envprint},
    {"file",file_op},
#ifndef WRITE_W25F
    {"fmerase",fmerase},
    {"fmrtest",fmrtest},
    {"fmwtest",fmwtest},
    {"go",go},
    {"gpiotest",gpiotest},
#endif
    {"help",print_help},
    {"history",history},
#ifndef WRITE_W25F
    //{"lcd19264init",lcd19264init},
    //{"lcd19264dc",dispcchar},
    {"kt",keytest},
    {"led",ledtest},
    {"logf",logflag},
    {"lst",lcdsuebinit},
    {"lstep",lcdsuebstep},
    {"lstest",lcdsuebtest},
    {"music",m_p},
#endif
    {"pm",print_mem},
    {"poff",poweroff},
    {"ps",ps},
    {"r",read_mem},
    {"reboot",reboot},
    {"rebootd",rebootd},
    {"rtc",rtc_cmd},
    {"rtcf",rtcf},
    {"rtcs",rtcs},
    {"sd",sd},
    {"stress",stress},
#ifndef WRITE_W25F
    //{"sdcmds",sd_cmds},
    {"test",test},
    {"taskmask",tm},
    //{"szk",show_ziku},
    //{"usb",usb_strg_init},
#endif
    {"w",write_mem},
    {"w25f",w25f},
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

char * str_to_str(char *s, char**result)
{
    while(*s == ' ')s++;
    *result=s;
    while(*s != ' ' && *s != 0)s++;
    if(*s==0){
        return NULL;
    }
    *s++=0;
    while(*s == ' ')s++;
    return s;
}

char * str_to_hex(char *s, uint32_t * result)
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
    uint length = 0x80, tmp;
    char *cp;

    tmp = get_howmany_para(p);
    if( tmp > 1)
        goto error;
    if(tmp == 0)
        goto print;
    str_to_hex(p, &length);
print:
    cp = (char *)mrw_addr;
    mem_print(cp, (uint32_t)mrw_addr, length);

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
    //record the history cmd
    review_cmd_his_index = cmdcache_index;
    strcpy((char*)cmd_caches[cmdcache_index++], cmd_buf);
    if(cmdcache_index==CMD_CACHES_SIZE){
        cmdcache_index = 0;
    }
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

extern unsigned long debug_enable;
uint time_limit_recv_byte(uint limit, char * c);
void run_cmd_interface()
{
    char c = 0, last_c = 0;
    int timeout = 5;

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
    memset(&cmd_caches[0][0], 0, CMD_CACHES_SIZE*COM_MAX_LEN);;
    cmd_buf_p = 0;
    lprintf_time("Enter CMD\n");
    lprint("\nCleanCMD>");

    while(!quit_cmd){
        last_c = c;
        c = con_recv();
        if(c == ENTER_CHAR || c == 0x1b || c== 0x03){
            if(c == ENTER_CHAR){
                handle_cmd();
                lprintf("\n");
            }
            if(c == 0x03){
                lprintf("^C\n");
            }
            if(c == 0x1b){
                lprintf("\n");
            }
            lmemset(cmd_buf, 0, COM_MAX_LEN);
            cmd_buf_p = 0;
            lprint("CleanCMD>");
        }
        else if(c == 0x08){
            if(!cmd_buf_p)
                continue;
            cmd_buf[--cmd_buf_p] = 0;
            lprintf("\b \b");
        }
        else if(c == 0x7f){
            if(!cmd_buf_p)
                continue;
            cmd_buf[--cmd_buf_p] = 0;
            con_send(c);
        }
        else if(c == 0x5B && last_c == 0x1b){//history cmd
            strcpy(cmd_buf, (char*)cmd_caches[review_cmd_his_index]);
            cmd_buf_p = strlen(cmd_buf);
            lprintf("%s", cmd_caches[review_cmd_his_index]);
            if(review_cmd_his_index==0){
                review_cmd_his_index=CMD_CACHES_SIZE;
            }
            review_cmd_his_index--;
        }
        else{
            if(cmd_buf_p < (COM_MAX_LEN - 1)){
                cmd_buf[cmd_buf_p++] = c;
                con_send(c);
            }
        }
    }
    quit_cmd = 0;
    lprintf_time("Quit CMD\n");
}
