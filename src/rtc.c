#include "common.h"
#include "rtc.h"

#define uchar unsigned char

uchar g8563_Store[6]; /*时间交换区,全局变量声明*/
uchar c8563_Store[6]={0x13,0x09,0x22,0x10,0x40,0x00}; /*写入时间初值：星期一 07:59:00*/

void SDA_set_input(uint8_t en);
/********************************************
  内部函数，延时1
 ********************************************/
void Delay()
{
    delay_us(10);
}
/********************************************
  内部函数，I2C开始
 ********************************************/
void Start()
{ 
    SDA_HIGH;
    SCL_HIGH;
    Delay();
    SDA_LOW;
    Delay();
    SCL_LOW;
}
/********************************************
  内部函数，I2C结束
 ********************************************/
void Stop()
{
    SDA_LOW;
    SCL_LOW;
    Delay();
    SCL_HIGH;
    Delay();
    SDA_HIGH;
    Delay();
}
/********************************************
  内部函数，输出ACK ,每个字节传输完成，输出ack=0,结束读书据，ack=1;
 ********************************************/
void WriteACK(uchar ack)
{
    if(ack)SDA_HIGH;
    else SDA_LOW;
    Delay();
    SCL_HIGH;
    Delay();
    SCL_LOW;
}
/********************************************
  内部函数，等待ACK
 ********************************************/
void WaitACK()
{  
    uchar errtime=20;
    SDA_HIGH;
    Delay(); /*读ACK*/
    SCL_HIGH;
    Delay();
    SDA_set_input(1);
    while(GET_SDA)
    {  
        errtime--;
        if(!errtime){
            lprintf("wait ack timeout\n");
            Stop();
        }
    }
    SCL_LOW;
    Delay();
    SDA_set_input(0);
}
/********************************************
  内部函数.输出数据字节
  入口:B=数据
 ********************************************/
void writebyte(uchar wdata)
{
    uchar i;
    lprintf("i2c write=%x\n", wdata);
    for(i=0;i<8;i++)
    {
        if(wdata&0x80) 
            SDA_HIGH;
        else 
            SDA_LOW;
        wdata<<=1;
        SCL_HIGH;
        Delay();
        SCL_LOW;
    }
    WaitACK();     //I2C器件或通讯出错，将会退出I2C通讯
}
/********************************************
  内部函数.输入数据
  出口:B
 ********************************************/
uchar Readbyte()
{
    uchar i,bytedata;
    SDA_HIGH;
    SDA_set_input(1);
    for(i=0;i<8;i++)
    {
        SCL_HIGH; 
        bytedata<<=1;
        bytedata|=GET_SDA;
        SCL_LOW;
        Delay();
    }
    SDA_set_input(0);
    lprintf("i2c read=%x\n", bytedata);
    return(bytedata);
}
/********************************************
  输出数据->pcf8563
 ********************************************/
void writeData(uchar address,uchar mdata)
{
    Start();
    writebyte(0xa2); /*写命令*/
    writebyte(address); /*写地址*/
    writebyte(mdata); /*写数据*/
    Stop();
}
/********************************************
  输入数据<-pcf8563
 ********************************************/
/*uchar ReadData(uchar address) //单字节
  {  
  uchar rdata;
  Start();
  writebyte(0xa2); //写命令
  writebyte(address); //写地址
  Start();
  writebyte(0xa3); //读命令
  rdata=Readbyte();
  WriteACK(1);
  Stop();
  return(rdata);
  }	*/
void ReadData1(uchar address,uchar count,uchar * buff) /*多字节*/
{  
    uchar i;
    Start();
    writebyte(0xa2); /*写命令*/
    writebyte(address); /*写地址*/
    Start();
    writebyte(0xa3); /*读命令*/
    for(i=0;i<count;i++)
    {
        buff[i]=Readbyte();
        if(i<count-1) 
            WriteACK(0);
    }
    WriteACK(1);
    Stop();
}  
/********************************************
  内部函数,读入时间到内部缓冲区
 ********************************************/
void P8563_Read(uint8_t*ip)
{   
    uchar time[7];
    uchar ict=7;
    ReadData1(0x02,0x07,time);
    ip[0]=time[0]&0x7f; /*秒 */
    ip[1]=time[1]&0x7f; /*分 */
    ip[2]=time[2]&0x3f; /*小时 */
    ip[3]=time[3]&0x3f; /*日 */
    ip[4]=time[4]&0x07; /*week */
    ip[5]=time[5]&0x9f; /*月 */
    ip[6]=time[6]; /*年  */

    while(ict--){
        lprintf("ip[%d]=%x\n", ict, ip[ict]);
    }
}
/********************************************
  读入时间到内部缓冲区----外部调用 
 ********************************************/
void P8563_gettime(uint8_t*ip)
{
    P8563_Read(ip);
    if(ip[0]==0)
        P8563_Read(ip); /*如果为秒=0，为防止时间变化，再读一次*/
}	
/********************************************
  写时间修改值
 ********************************************/
void P8563_settime(uint8_t*ip)
{
    //uchar i;
    writeData(8,ip[0]); //年 
    writeData(7,ip[1]); //月 
    writeData(5,ip[2]); //日 
    writeData(4,ip[3]); //时 
    writeData(3,ip[4]); //分  
    writeData(2,ip[5]); //秒 
    delay_ms(1000);
    //P8563_gettime(g8563_Store);
}

void SDA_set_input(uint8_t en)
{
    GPIO_InitTypeDef GPIO_InitStructure;	//GPIO

    GPIO_InitStructure.GPIO_Pin = SDA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        if(en){
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;  //上拉输入
            GPIO_Init(SDA_GG, &GPIO_InitStructure);
        }
        else{
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出 
            GPIO_Init(SDA_GG, &GPIO_InitStructure);
        }
    GPIO_SetBits(SDA_GG, SDA_PIN);	
}

/********************************************
  P8563的初始化-----外部调用
 ********************************************/
void P8563_init()
{
    //uchar i;


    GPIO_InitTypeDef GPIO_InitStructure;	//GPIO
    
        //注意,时钟使能之后,对GPIO的操作才有效
        //所以上拉之前,必须使能时钟.才能实现真正的上拉输出
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = SDA_PIN|SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB,SDA_PIN|SCL_PIN);
    
        // P8563_settime();
#if 0
        for(i=0;i<=5;i++) g8563_Store[i]=c8563_Store[i]; /*初始化时间*/
    P8563_settime();  
#endif
    // if((ReadData(0x0a)&0x3f)!=0x08) /*检查是否第一次启动，是则初始化时间*/
    // {
    //	    P3_4 = 0;
    //       for(i=0;i<=3;i++) g8563_Store[i]=c8563_Store[i]; /*初始化时间*/
    //       P8563_settime();
    //       writeData(0x0,0x00);
    //       writeData(0xa,0x8); /*8:00报警*/
    //       writeData(0x1,0x12); /*报警有效*/
    //      writeData(0xd,0xf0);  //编程输出32.768K的频率
    //  }
}
void rtc_write(uint8_t*ip)
{
    P8563_init();
    P8563_settime(ip);
}
void rtc_read(uint8_t*ip)
{
    P8563_init();
    P8563_gettime(ip);
}
static char t_d[24];
uint8_t hex2bcd(uint8_t ipt)
{
    return (ipt/10)*0x10 + (ipt%10);
}
uint8_t bcd2hex(uint8_t ipt)
{
    return (ipt&0xf0)*10 + (ipt&0xf);
}
char* get_rtc_time(date_info_t*dit)
{
    uint8_t time_date[7];
    rtc_read(time_date);
    memset(t_d, 0, 24);
    if(time_date[5] & 0x80){
        t_d[0] = '1';
        t_d[1] = '9';
        if(dit!=0){
            dit->year = 1900 + bcd2hex(time_date[6]);
        }
    }
    else{
        t_d[0] = '2';
        t_d[1] = '0';
        if(dit!=0){
            dit->year = 2000 + bcd2hex(time_date[6]);
        }
    }
    slprintf(&t_d[2], "%b.%b.%b W%x %b:%b:%b",
            time_date[6], time_date[5]&0x1f, time_date[3],
            time_date[4], time_date[2],
            time_date[1], time_date[0]);
    if(dit!=0){
        dit->month = bcd2hex(time_date[5]&0x1f);
        dit->day = bcd2hex(time_date[3]);
        dit->weekday = time_date[4];
        dit->hour = bcd2hex(time_date[2]);
        dit->minute = bcd2hex(time_date[1]);
        dit->second = bcd2hex(time_date[0]);
    }
    lprintf("dit:%d.%d.%d %d:%d:%d week%d\n",
            dit->year, dit->month, dit->day,
            dit->hour, dit->minute, dit->second, dit->weekday);
    lprintf("%s\n", t_d);
    return t_d;
}
uint8_t rtc_read_reg(uint8_t addr)
{
    uint8_t ret;
    ReadData1(addr,1,&ret);
    return ret;
}

void rtc_write_reg(uint8_t addr, uint8_t data)
{
    writeData(addr,data);
}

uint32_t diff_with_inc_step(uint32_t f, uint32_t b, uint32_t inc_step)
{
    return (f>=b)?f-b:f+inc_step-b;
}
uint32_t add_with_back_limit(uint32_t * iptp, uint32_t diff, uint32_t limit)
{
    uint32_t ret = 0;
    uint32_t t = *iptp + diff;
    if(t>=limit){
        ret = 1;
    }
    *iptp = ret?t-limit:t;
    return ret;
}
uint32_t time_diff_minutes(date_info_t* dtp_f, date_info_t * dtp)
{
    uint32_t ret, h = dtp_f->hour;
    ret = diff_with_inc_step(dtp_f->minute, dtp->minute, 60);
    if(dtp_f->minute < dtp->minute){
        h = diff_with_inc_step(h, 1, 24);
    }
    ret += diff_with_inc_step(h, dtp->hour, 24)*60;
    return ret;
}

void add_time_diff_minutes(date_info_t*dtp, uint32_t tsms)
{
    dtp->hour += add_with_back_limit(&dtp->minute, tsms, 60);
}
void auto_time_alert_set(uint32_t time_step_minutes)
{
    date_info_t dt, dt_alt;
    uint32_t h;

    get_rtc_time(&dt);
    dt_alt.hour = bcd2hex(rtc_read_reg(0x0a));
    h = dt_alt.hour;
    dt_alt.minute = bcd2hex(rtc_read_reg(0x09));
    while(time_diff_minutes(&dt_alt, &dt)>time_step_minutes)
    {
        add_time_diff_minutes(&dt_alt, time_step_minutes);
    }
    if(h != dt_alt.hour){
        rtc_write_reg(0x0a, hex2bcd(dt_alt.hour));
    }
    lcd_lprintf(190, 150, "Next auto power on: %d:%d", dt_alt.hour, dt_alt.minute);
}
