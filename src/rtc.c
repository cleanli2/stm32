#include "common.h"
#include "rtc.h"

#define uchar unsigned char

int use_SCL_1=0;
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
uint8_t WaitACK()
{  
    uint8_t ret = 1;
    uchar errtime=20;
    SDA_HIGH;
    Delay(); /*读ACK*/
    SDA_set_input(1);
    SCL_HIGH;
    Delay();
    while(GET_SDA)
    {  
        errtime--;
        if(!errtime){
            lprintf("wait ack timeout\n");
            ret = 0;
            break;
        }
    }
    SCL_LOW;
    Delay();
    SDA_set_input(0);
    SDA_HIGH;
    return ret;
}
/********************************************
  内部函数.输出数据字节
  入口:B=数据
 ********************************************/
uint8_t writebyte(uchar wdata)
{
    uchar i;
    //lprintf("i2c write=%x\n", wdata);
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
        Delay();
    }
    return WaitACK();     //I2C器件或通讯出错，将会退出I2C通讯
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
        Delay();
        bytedata<<=1;
        bytedata|=GET_SDA;
        SCL_LOW;
        Delay();
    }
    SDA_set_input(0);
    //lprintf("i2c read=%x\n", bytedata);
    return(bytedata);
}
/********************************************
  输出数据->pcf8563
 ********************************************/
uint8_t writeData(uchar address,uchar mdata)
{
    uint8_t ret;
    Start();
    ret=writebyte(0xa2); /*写命令*/
    if(ret==0)goto err;
    ret=writebyte(address); /*写地址*/
    if(ret==0)goto err;
    ret=writebyte(mdata); /*写数据*/
    Stop();
    return ret;
err:
    Stop();
    lprintf("writeData addr %b data %b error\n", address, mdata);
    return ret;
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
uint8_t ReadData1(uchar address,uchar count,uchar * buff) /*多字节*/
{  
    uint8_t ret;
    uchar i;
    Start();
    ret=writebyte(0xa2); /*写命令*/
    if(ret==0)goto err;
    ret=writebyte(address); /*写地址*/
    if(ret==0)goto err;
    Start();
    ret=writebyte(0xa3); /*读命令*/
    if(ret==0)goto err;
    for(i=0;i<count;i++)
    {
        buff[i]=Readbyte();
        if(i<count-1) 
            WriteACK(0);
    }
    WriteACK(1);
err:
    Stop();
    return ret;
}  
/********************************************
  内部函数,读入时间到内部缓冲区
 ********************************************/
uint8_t P8563_Read(uint8_t*ip)
{   
    uint8_t ret = 0, maxtry = 3;
    uchar time[7];
    //uchar ict=7;
    while(maxtry--){
        ret = ReadData1(0x02,0x07,time);
        if(ret)break;
    }
    if(!ret){
        lprintf("RTC read error\n");
        ip[0]= 0x00;
        ip[1]= 0x01;
        ip[2]= 0x02;
        ip[3]= 0x01;
        ip[4]= 0x01;
        ip[5]= 0x09; /*月 */
        ip[6]= 0x22; /*年  */
        return ret;
    }
    if(time[0]&0x80){
        lprintf("VL is set, clear it\n");
        rtc_write_reg(2,time[0]&0x7f); //clear VL
    }
    ip[0]=time[0]&0x7f; /*秒 */
    ip[1]=time[1]&0x7f; /*分 */
    ip[2]=time[2]&0x3f; /*小时 */
    ip[3]=time[3]&0x3f; /*日 */
    ip[4]=time[4]&0x07; /*week */
    ip[5]=time[5]&0x9f; /*月 */
    ip[6]=time[6]; /*年  */

#if 0
    while(ict--){
        lprintf("ip[%d]=%x\n", ict, ip[ict]);
    }
#endif
    return ret;
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
    rtc_write_reg(8,ip[0]); //年 
    rtc_write_reg(7,ip[1]); //月 
    rtc_write_reg(5,ip[2]); //日 
    rtc_write_reg(4,ip[3]); //时 
    rtc_write_reg(3,ip[4]); //分  
    rtc_write_reg(6,ip[5]); //秒 
    rtc_write_reg(2, 0); //秒 
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
    static int rtc_inited = 0;
    //uchar i;

    if(rtc_inited)return;

    GPIO_InitTypeDef GPIO_InitStructure;	//GPIO
    RCC_APB2PeriphClockCmd(I2C_GPIO_PERIPH, ENABLE);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    for(use_SCL_1=0;use_SCL_1!=2;use_SCL_1++){
        lprintf("try use_SCL_1=%d\n", use_SCL_1);
        GPIO_InitStructure.GPIO_Pin = SDA_PIN|SCL_PIN;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出 
        GPIO_Init(I2C_GROUP, &GPIO_InitStructure);
        GPIO_SetBits(I2C_GROUP,SDA_PIN|SCL_PIN);
        if(1==rtc_write_reg(0x0,0x00)){
            lprintf("try use_SCL_1=%d OK\n", use_SCL_1);
            puthexch(rtc_read_reg(1));
            rtc_write_reg(0x1,0x10|rtc_read_reg(0x1)); /*报警有效*/
            rtc_inited = 1;
            break;
        }
    }
}
void rtc_write(uint8_t*ip)
{
    //P8563_init();
    //P8563_settime(ip);
}
void rtc_read(uint8_t*ip)
{
#if 0
    P8563_init();
    P8563_gettime(ip);
#else
    ip[0]=0;
    ip[1]=0;
    ip[2]=0;
    ip[3]=1;
    ip[4]=1;
    ip[5]=0x81;
    ip[6]=0x25;
#endif
}
char t_d[24];
uint8_t hex2bcd(uint8_t ipt)
{
    return (ipt/10)*0x10 + (ipt%10);
}
uint32_t bcd2hex_32(uint32_t ipt)
{
    uint32_t bv=1, ret=0;
    for(int i = 0;i<8;i++){
        ret+=bv*(ipt&0xf);
        ipt>>=4;
        bv*=10;
    }
    return ret;
}
uint8_t bcd2hex(uint8_t ipt)
{
    return ((ipt&0xf0)>>4)*10 + (ipt&0xf);
}
#ifdef RTC_8563
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
    slprintf(&t_d[2], "%b.%b.%b %b:%b:%b W%x",
            time_date[6], time_date[5]&0x1f, time_date[3],
            time_date[2], time_date[1], time_date[0], time_date[4]);
    if(dit!=0){
        dit->month = bcd2hex(time_date[5]&0x1f);
        dit->day = bcd2hex(time_date[3]);
        dit->weekday = time_date[4];
        dit->hour = bcd2hex(time_date[2]);
        dit->minute = bcd2hex(time_date[1]);
        dit->second = bcd2hex(time_date[0]);
#if 0
        lprintf("dit:%d.%d.%d %d:%d:%d week%d\n",
                (uint32_t)dit->year, (uint32_t)dit->month, (uint32_t)dit->day,
                (uint32_t)dit->hour, (uint32_t)dit->minute, (uint32_t)dit->second, (uint32_t)dit->weekday);
#endif
    }
    //lprintf("%s\n", t_d);
    return t_d;
}
void get_date(date_info_t*dit)
{
    uint8_t time_date[7];
    if(dit){
        rtc_read(time_date);
        dit->month = bcd2hex(time_date[5]&0x1f);
        dit->day = bcd2hex(time_date[3]);
        dit->weekday = time_date[4];
        dit->hour = bcd2hex(time_date[2]);
        dit->minute = bcd2hex(time_date[1]);
        dit->second = bcd2hex(time_date[0]);
    }
}
#endif
uint8_t rtc_read_reg(uint8_t addr)
{
    uint8_t ret;
    ReadData1(addr,1,&ret);
    return ret;
}

void rtc_dump_regs()
{
    uint8_t regs[16];
    lprintf("dump rtc regs\n");
    ReadData1(0,16,regs);
    mem_print((char*)regs, 0, 16);
}

uint8_t rtc_write_reg(uint8_t addr, uint8_t data)
{
    uint8_t ret = 0, maxtry = 3;
    while(maxtry--){
        ret = writeData(addr,data);
        if(ret)break;
    }
    return ret;
}


void auto_time_alert_set(uint32_t time_step_minutes, int show_x, int show_y)
{
    date_info_t dt, dt_alt;
    uint32_t h, m;
    uint8_t reg1;

    reg1 = rtc_read_reg(0x1);
    if(0==get_env_uint("autopon", 0)){
        rtc_write_reg(0x1,0xfd&reg1); /*alert disable*/
        if(show_x>0 && show_y>0){
            lcd_lprintf(show_x, show_y, "AutoPowon:off   ");
        }
        return;
    }
    rtc_write_reg(0x1,0x02|reg1); /*alert enable*/
    get_rtc_time(&dt);
    dt_alt.hour = bcd2hex(rtc_read_reg(0x0a));
    h = dt_alt.hour;
    dt_alt.minute = bcd2hex(rtc_read_reg(0x09));
    m = dt_alt.minute;
    while(time_diff_minutes(&dt_alt, &dt)>time_step_minutes)
    {
        add_time_diff_minutes(&dt_alt, time_step_minutes);
    }
    if(h != dt_alt.hour){
        rtc_write_reg(0x0a, hex2bcd(dt_alt.hour));
    }
    if(m != dt_alt.minute){
        rtc_write_reg(0x09, hex2bcd(dt_alt.minute));
    }
    lprintf_time("Next auto power on: %b:%b\n", hex2bcd(dt_alt.hour), hex2bcd(dt_alt.minute));
    if(show_x>0 && show_y>0){
        lcd_lprintf(show_x, show_y, "AutoPowon:%b:%b", hex2bcd(dt_alt.hour), hex2bcd(dt_alt.minute));
    }
}

uint8_t check_rtc_alert_and_clear()
{
    uint8_t ret = 0, reg;
    reg = rtc_read_reg(1);
    lprintf("rtcreg1=%x\n", reg);
    ret = reg&0x08;
    if(ret){
        lprintf_time("rtc flag set found!!!!!!!!!\n");
        rtc_write_reg(1,0x12);//clear rtc int pin
    }
    return ret;
}

uint adjust_1min(uint faster_1min)
{
#ifdef RTC_8563
    lprintf("ad1min %d\n", faster_1min);
    uint8_t scd = bcd2hex(rtc_read_reg(SECOND_REG));
    if(scd != bcd2hex(rtc_read_reg(SECOND_REG))){
        lprintf("read rtc reg 'second' fail\n");
        return RTC_FAIL;
    }
    lprintf("second=%d\n", scd);
    if(scd>=58){
        lprintf(">=58 fail\n");
        return RTC_FAIL;
    }
    uint8_t min = bcd2hex(rtc_read_reg(MINUTE_REG));
    if(min != bcd2hex(rtc_read_reg(MINUTE_REG)) || min>59){
        lprintf("read rtc reg 'min' fail\n");
        return RTC_FAIL;
    }
    lprintf("min=%d\n", min);
    if((faster_1min && min == 59) ||
            (!faster_1min && min == 0)){
        lprintf("=59 or =0 fail\n");
        return RTC_FAIL;
    }
    faster_1min?min++:min--;
    lprintf("W:min=%d 0x%b\n", min, hex2bcd(min));
    return rtc_write_reg(MINUTE_REG, hex2bcd(min));
#else
    adjust_second(faster_1min?60:-60);
    return RTC_OK;
#endif
}

void clear_second()
{
#ifdef RTC_8563
    rtc_write_reg(SECOND_REG, 0);
#else
    //rtc_write_reg(SECOND_REG, 0);
    date_info_t dt;
    int s;

    get_rtc_time(&dt);
    s = 0 - dt.second;
    adjust_second(s);
#endif
}

int32_t get_t_t_n_c(int disp)
{
    int32_t ret;
    char ch_t[ENV_MAX_VALUE_LEN], *p=&ch_t[0];
    date_info_t dt, dt_lastadj;
    uint32_t hours_adj_1min, diff_hours, v_tmp;
    int32_t ha1m;

    lprintf_time("get_time_correct +\n");
    if(ENV_OK != get_env("LastTimeAdj", ch_t)){
        lprintf_time("skip ATC 1\n");
        return 0;
    }
    //replace '.' & ':' with space
    for(int i=0;i<32;i++){
        if(ch_t[i]=='.' || ch_t[i]==':'){
            ch_t[i]=' ';
        }
    }
    p=&ch_t[0];
    p = str_to_hex(p, &v_tmp);
    dt_lastadj.year = v_tmp;
    p = str_to_hex(p, &v_tmp);
    dt_lastadj.month = v_tmp;
    p = str_to_hex(p, &v_tmp);
    dt_lastadj.day = v_tmp;
    p = str_to_hex(p, &v_tmp);
    dt_lastadj.hour = v_tmp;
    p = str_to_hex(p, &v_tmp);
    dt_lastadj.minute = v_tmp;
    p = str_to_hex(p, &v_tmp);
    dt_lastadj.second = v_tmp;
    p = str_to_hex(p, &v_tmp);
    dt_lastadj.weekday = v_tmp;

    if(ENV_OK != get_env("HsAdj1Min", ch_t) ||
            (ch_t[0]!='+' && ch_t[0]!='-') ){
        lprintf_time("skip ATC 2\n");
        return 0;
    }
    //'-169' means slower 1 min or '+169' faster
    p=&ch_t[1];
    p = str_to_hex((char*)p, &hours_adj_1min);
    if(0==hours_adj_1min){
        lprintf_time("skip ATC 3\n");
        return 0;
    }

    //convert
    dt_lastadj.year = bcd2hex_32(dt_lastadj.year);
    dt_lastadj.month= bcd2hex(dt_lastadj.month);
    dt_lastadj.day    = bcd2hex(dt_lastadj.day    );
    dt_lastadj.hour   = bcd2hex(dt_lastadj.hour   );
    dt_lastadj.minute = bcd2hex(dt_lastadj.minute );
    dt_lastadj.second = bcd2hex(dt_lastadj.second );
    dt_lastadj.weekday= bcd2hex(dt_lastadj.weekday);
    lprintf_time("get last adj:%d %d %d %d %d %d %d\n",
        dt_lastadj.year,
        dt_lastadj.month,
        dt_lastadj.day  ,
        dt_lastadj.hour ,
        dt_lastadj.minute,
        dt_lastadj.second,
        dt_lastadj.weekday);
    hours_adj_1min = bcd2hex_32(hours_adj_1min);
    lprintf_time("hours adj %c%d\n", ch_t[0], hours_adj_1min);

    lprintf_time("curtime:%s\n",get_rtc_time(&dt));
    /*
    if(!is_later_than(&dt, &dt_lastadj)){
        lprintf("skip ATC 3\n");
        return;
    }
    */
    diff_hours = time_diff_hours(&dt, &dt_lastadj);
    lprintf_time("diff hours %d\n", diff_hours);
    if(disp) lcd_lprintf(340, 610, "HoursPass:%d", diff_hours);

    ret = 60*diff_hours/hours_adj_1min;
    lprintf("ret %d\n", ret);
    ha1m=hours_adj_1min;
    if(ch_t[0]=='-'){
        ret = -ret;
        ha1m=-hours_adj_1min;
    }
    lprintf("HsAdj1Min %d second_fix %d\n", ha1m, ret);
    return ret;
    /*
    if(diff_hours>hours_adj_1min){
        if(ch_t[0]=='+'){
            lprintf_time("try faster 1min\n");
        }
        else{
            lprintf_time("try slower 1min\n");
        }
        if(RTC_OK==adjust_1min(ch_t[0]=='+')){
            set_env("LastTimeAdj", get_rtc_time(NULL));
            lprintf_time("adj OK\n");
        }
        else{
            lprintf_time("adj fail\n");
        }
    }
    */
}

#define MAGIC_TIME_NEED_CORRECT 203250
#define DELAY_TIME_CORRECT 10
#define MAX_SECONDS_TIME_CORRECT 55
void do_time_correct()
{
    static int32_t time_need_correct_s = MAGIC_TIME_NEED_CORRECT;
    int32_t t_s;
    int32_t TRIGGER_SECONDS_TIME_CORRECT;

    if(0 == time_need_correct_s){
        return;
    }
    if(time_need_correct_s > MAGIC_TIME_NEED_CORRECT-DELAY_TIME_CORRECT){
        lprintf("time corrt skip\n");
        time_need_correct_s--;
        return;
    }
    if(time_need_correct_s == MAGIC_TIME_NEED_CORRECT-DELAY_TIME_CORRECT){
        time_need_correct_s = get_t_t_n_c(0);
        lprintf_time("time corrt = %d\n", time_need_correct_s);
    }
    lprintf_time("+geu\n");
    TRIGGER_SECONDS_TIME_CORRECT = (int)get_env_uint("time_ct", 20);
    lprintf_time("-geu\n");
    if(time_need_correct_s < TRIGGER_SECONDS_TIME_CORRECT &&
            time_need_correct_s > -TRIGGER_SECONDS_TIME_CORRECT){
        lprintf_time("It's not time yet for time correct, trigger %d\n",
                TRIGGER_SECONDS_TIME_CORRECT);
        time_need_correct_s = 0;
        return;
    }
    if(time_need_correct_s > MAX_SECONDS_TIME_CORRECT ||
            time_need_correct_s < -MAX_SECONDS_TIME_CORRECT){
        time_need_correct_s = 0;
        lprintf_time("It's too much for time correct, max %d\n",
                MAX_SECONDS_TIME_CORRECT);
        return;
    }
    t_s = bcd2hex(rtc_read_reg(SECOND_REG));
    lprintf_time("cur time sec  %d\n", t_s);
    t_s += time_need_correct_s;
    if(t_s > 2 && t_s < 58){
        if(rtc_write_reg(SECOND_REG, hex2bcd(t_s))){
            //write OK
            reset_time_offset();
            lprintf_time("time fix OK %d\n", t_s);
            time_need_correct_s = 0;
            set_env("LastTimeAdj", get_rtc_time(NULL));
        }
        else{
            lprintf_time("time fix fail, wait next\n");
        }
    }
    else{
        lprintf_time("corrected time sec %d, out of range, wait next\n", t_s);
    }
}

uint8_t pcf8574t_writeData(uchar mdata)
{
    uint8_t ret;
    Start();
    ret=writebyte(0x40); /*写命令*/
    if(ret==0)goto err;
    ret=writebyte(mdata); /*写数据*/
    Stop();
    return ret;
err:
    Stop();
    lprintf("pcf8574t writeData data %b error\n", mdata);
    return ret;
}
uint8_t pcf8574t_readData1(uchar * buff) /*多字节*/
{  
    uint8_t ret;
    Start();
    ret=writebyte(0x41); /*读命令*/
    if(ret==0){
        lprintf("pcf8574t readData data error\n");
        goto err;
    }
    buff[0]=Readbyte();
    WriteACK(1);
    return ret;
err:
    Stop();
    return ret;
}  
