#include "common.h"
#include "rtc.h"
#include "date.h"

#define uchar unsigned char

uchar g8563_Store[6]; /*ʱ�佻����,ȫ�ֱ�������*/
uchar c8563_Store[6]={0x13,0x09,0x22,0x10,0x40,0x00}; /*д��ʱ���ֵ������һ 07:59:00*/

void i2c_init();
static int rtc_inited = 0;
void SDA_set_input(uint8_t en);
/********************************************
  �ڲ���������ʱ1
 ********************************************/
void Delay()
{
    delay_us(1);
}
/********************************************
  �ڲ�������I2C��ʼ
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
  �ڲ�������I2C����
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
  �ڲ����������ACK ,ÿ���ֽڴ�����ɣ����ack=0,��������ݣ�ack=1;
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
  �ڲ��������ȴ�ACK
 ********************************************/
uint8_t WaitACK()
{  
    uint8_t ret = 1;
    uchar errtime=20;
    SDA_HIGH;
    Delay(); /*��ACK*/
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
    return ret;
}
/********************************************
  �ڲ�����.��������ֽ�
  ���:B=����
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
    return WaitACK();     //I2C������ͨѶ���������˳�I2CͨѶ
}
/********************************************
  �ڲ�����.��������
  ����:B
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
  �������->pcf8563
 ********************************************/
uint8_t writeData(uchar address,uchar mdata)
{
    uint8_t ret;
    Start();
    ret=writebyte(0xa2); /*д����*/
    if(ret==0)goto err;
    ret=writebyte(address); /*д��ַ*/
    if(ret==0)goto err;
    ret=writebyte(mdata); /*д����*/
    Stop();
    return ret;
err:
    Stop();
    lprintf("writeData addr %b data %b error\n", address, mdata);
    return ret;
}
/********************************************
  ��������<-pcf8563
 ********************************************/
/*uchar ReadData(uchar address) //���ֽ�
  {  
  uchar rdata;
  Start();
  writebyte(0xa2); //д����
  writebyte(address); //д��ַ
  Start();
  writebyte(0xa3); //������
  rdata=Readbyte();
  WriteACK(1);
  Stop();
  return(rdata);
  }	*/
uint8_t ReadData1(uchar address,uchar count,uchar * buff) /*���ֽ�*/
{  
    uint8_t ret;
    uchar i;
    Start();
    ret=writebyte(0xa2); /*д����*/
    if(ret==0)goto err;
    ret=writebyte(address); /*д��ַ*/
    if(ret==0)goto err;
    Start();
    ret=writebyte(0xa3); /*������*/
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
int cam_w_reg(uint8_t addr, uint8_t data)
{
    int ret;
    Start();
    ret=writebyte(0x42); /*д����*/
    if(ret==0)goto err;
    ret=writebyte(addr); /*д��ַ*/
    if(ret==0)goto err;
    ret=writebyte(data); /*д����*/
    Stop();
    return ret;
err:
    Stop();
    lprintf("cam writeData addr %b data %b error\n", addr, data);
    return ret;
}
uint8_t cam_r_regn(uchar addr,uchar n,uchar * buf) /*���ֽ�*/
{  
    uint8_t ret;
    uchar i;
    Start();
    ret=writebyte(0x42); /*д����*/
    if(ret==0)goto err;
    ret=writebyte(addr); /*д��ַ*/
    if(ret==0)goto err;
    Stop();
    Start();
    ret=writebyte(0x43); /*������*/
    if(ret==0)goto err;
    for(i=0;i<n;i++)
    {
        buf[i]=Readbyte();
        if(i<n-1) 
            WriteACK(0);
    }
    WriteACK(1);
err:
    Stop();
    return ret;
}  

/********************************************
  �ڲ�����,����ʱ�䵽�ڲ�������
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
        return ret;
    }
    ip[0]=time[0]&0x7f; /*�� */
    ip[1]=time[1]&0x7f; /*�� */
    ip[2]=time[2]&0x3f; /*Сʱ */
    ip[3]=time[3]&0x3f; /*�� */
    ip[4]=time[4]&0x07; /*week */
    ip[5]=time[5]&0x9f; /*�� */
    ip[6]=time[6]; /*��  */

#if 0
    while(ict--){
        lprintf("ip[%d]=%x\n", ict, ip[ict]);
    }
#endif
    return ret;
}
/********************************************
  ����ʱ�䵽�ڲ�������----�ⲿ���� 
 ********************************************/
void P8563_gettime(uint8_t*ip)
{
    P8563_Read(ip);
    if(ip[0]==0)
        P8563_Read(ip); /*���Ϊ��=0��Ϊ��ֹʱ��仯���ٶ�һ��*/
}	
/********************************************
  дʱ���޸�ֵ
 ********************************************/
void P8563_settime(uint8_t*ip)
{
    //uchar i;
    rtc_write_reg(8,ip[0]); //�� 
    rtc_write_reg(7,ip[1]); //�� 
    rtc_write_reg(5,ip[2]); //�� 
    rtc_write_reg(4,ip[3]); //ʱ 
    rtc_write_reg(3,ip[4]); //��  
    rtc_write_reg(2,ip[5]); //�� 
    delay_ms(1000);
    //P8563_gettime(g8563_Store);
}

void SDA_set_input(uint8_t en)
{
    GPIO_InitTypeDef GPIO_InitStructure;	//GPIO

    GPIO_InitStructure.GPIO_Pin = SDA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        if(en){
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;  //��������
            GPIO_Init(SDA_GG, &GPIO_InitStructure);
        }
        else{
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //������� 
            GPIO_Init(SDA_GG, &GPIO_InitStructure);
        }
    GPIO_SetBits(SDA_GG, SDA_PIN);	
}

/********************************************
  P8563�ĳ�ʼ��-----�ⲿ����
 ********************************************/
void P8563_init()
{
    static int rtc_inited = 0;
    //uchar i;

    if(rtc_inited)return;

    i2c_init();
    
#if 0
        for(i=0;i<=5;i++) g8563_Store[i]=c8563_Store[i]; /*��ʼ��ʱ��*/
    P8563_settime();  
#endif
    // if((ReadData(0x0a)&0x3f)!=0x08) /*����Ƿ��һ�������������ʼ��ʱ��*/
    // {
    //	    P3_4 = 0;
    //       for(i=0;i<=3;i++) g8563_Store[i]=c8563_Store[i]; /*��ʼ��ʱ��*/
    //       P8563_settime();
    puthexch(rtc_read_reg(0));
    puthexch(rtc_read_reg(1));
    rtc_write_reg(0x0,0x00);
    //       writeData(0xa,0x8); /*8:00����*/
    rtc_write_reg(0x1,0xfd&rtc_read_reg(0x1));//disable alerm
    //      writeData(0xd,0xf0);  //������32.768K��Ƶ��
    //  }
    rtc_inited = 1;
}
void rtc_write(uint8_t*ip)
{
    lprintf_time("rtc settime:%b %b %b %b %b %b\n",
            ip[0], ip[1],ip[2], ip[3],ip[4], ip[5]);
    P8563_init();
    P8563_settime(ip);
}
void rtc_read(uint8_t*ip)
{
    P8563_init();
    P8563_gettime(ip);
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
        lcd_lprintf(1, show_x, show_y, "Next auto power on: %b:%b", hex2bcd(dt_alt.hour), hex2bcd(dt_alt.minute));
    }
}

void auto_time_correct_raw(int adj_type)
{
    char ch_t[ENV_MAX_VALUE_LEN], *p=&ch_t[0];
    date_info_t dt, dt_lastadj;
    uint32_t hours_adj_1min, diff_hours, v_tmp;
    if(ENV_OK != get_env("LastTimeAdj", ch_t)){
        lprintf_time("ATC:t=%d skip 1\n", adj_type);
        return;
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
        lprintf_time("ATC:t=%d skip 2\n", adj_type);
        return;
    }
    //'-169' means slower 1 min or '+169' faster
    p=&ch_t[1];
    p = str_to_hex((char*)p, &hours_adj_1min);

    //convert
    dt_lastadj.year = bcd2hex_32(dt_lastadj.year);
    dt_lastadj.month= bcd2hex(dt_lastadj.month);
    dt_lastadj.day    = bcd2hex(dt_lastadj.day    );
    dt_lastadj.hour   = bcd2hex(dt_lastadj.hour   );
    dt_lastadj.minute = bcd2hex(dt_lastadj.minute );
    dt_lastadj.second = bcd2hex(dt_lastadj.second );
    dt_lastadj.weekday= bcd2hex(dt_lastadj.weekday);
    hours_adj_1min = bcd2hex_32(hours_adj_1min);

    lprintf("ATC:curtime:%s tp %d\n",get_rtc_time(&dt), adj_type);
    if(!is_later_than(&dt, &dt_lastadj)){
        lprintf_time("ATC:t=%d skip 3\n", adj_type);
        return;
    }
    diff_hours = time_diff_hours(&dt, &dt_lastadj);
    //lcd_lprintf(1, 340, 610, "HoursPass:%d", diff_hours);

    //do adjust
    switch(adj_type){
        case ADJ_10SECS:
            {
                if(diff_hours>hours_adj_1min/6){
                    lprintf_time("ATC:curtime:%s\n",get_rtc_time(NULL));
                    lprintf_time("get last adj:%d %d %d %d %d %d %d\n",
                            dt_lastadj.year,
                            dt_lastadj.month,
                            dt_lastadj.day  ,
                            dt_lastadj.hour ,
                            dt_lastadj.minute,
                            dt_lastadj.second,
                            dt_lastadj.weekday);
                    lprintf_time("hours adj %c%d\n", ch_t[0], hours_adj_1min);
                    lprintf_time("diff hours %d\n", diff_hours);
                    if(ch_t[0]=='+'){
                        lprintf_time("try faster 10 seconds\n");
                    }
                    else{
                        lprintf_time("try slower 10 seconds\n");
                    }
                    if(RTC_OK==adjust_10s(ch_t[0]=='+')){
                        set_env("LastTimeAdj", get_rtc_time(NULL));
                        lprintf_time("adj OK\n");
                    }
                    else{
                        lprintf_time("adj fail\n");
                    }
                }
            }
            break;
        case ADJ_LESS_THAN_1HOUR: 
            {
                int32_t seconds_adj;
                lprintf("ADJ_LESS_THAN_1HOUR:diffhour %d ha1m %d\n",
                        diff_hours, hours_adj_1min);
                if(diff_hours>hours_adj_1min/6){
                    seconds_adj = diff_hours * 60 /hours_adj_1min;
                    if(ch_t[0]=='-')seconds_adj=-seconds_adj;
                    if(RTC_OK==adjust_time(seconds_adj)){
                        set_env("LastTimeAdj", get_rtc_time(NULL));
                        lprintf_time("adj %d seconds OK\n", seconds_adj);
                    }
                    else{
                        lprintf_time("adj %d seconds fail\n", seconds_adj);
                    }
                }
            }
            break;
        case ADJ_MINUTE:
        default:
            {
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
            }
            break;
    }
}

void auto_time_correct_10s()
{
    auto_time_correct_raw(ADJ_10SECS);
}

void auto_time_correct()
{
    auto_time_correct_raw(ADJ_MINUTE);
}

void auto_time_correct2()
{
    auto_time_correct_raw(ADJ_LESS_THAN_1HOUR);
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

uint adjust_10s(uint faster)
{
#ifdef RTC_8563
    uint ret;
    lprintf("adj10s faster %d\n", faster);
    uint8_t scd = bcd2hex(rtc_read_reg(SECOND_REG));
    if(scd != bcd2hex(rtc_read_reg(SECOND_REG))){
        lprintf("read rtc reg 'second' fail\n");
        return RTC_FAIL;
    }
    if(scd>=45 && faster){
        lprintf("second=%d\n", scd);
        lprintf(">=45 fail\n");
        return RTC_FAIL;
    }
    if(scd<=15 && !faster){
        lprintf("second=%d\n", scd);
        lprintf("<=15 fail\n");
        return RTC_FAIL;
    }
    if(scd%10>7){
        lprintf("second=%d\n", scd);
        lprintf(">7 fail\n");
        return RTC_FAIL;
    }
    if(faster){
        scd+=10;
    }
    else{
        scd-=10;
    }
    ret = rtc_write_reg(SECOND_REG, hex2bcd(scd));
    lprintf("W:sec=%d 0x%b\n", scd, hex2bcd(scd));
    return ret;
#else
    adjust_second(faster?10:-10);
    return RTC_OK;
#endif
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

uint adjust_time(int scds)
{
#ifdef RTC_8563
    uint8_t h, m, s;
    uint8_t hb, mb, sb;
    if(scds>=3600)return RTC_FAIL;
    s = bcd2hex(rtc_read_reg(SECOND_REG));
    if(s != bcd2hex(rtc_read_reg(SECOND_REG))){
        lprintf("read rtc reg 'second' fail\n");
        return RTC_FAIL;
    }
    m = bcd2hex(rtc_read_reg(MINUTE_REG));
    if(m != bcd2hex(rtc_read_reg(MINUTE_REG))){
        lprintf("read rtc reg 'min' fail\n");
        return RTC_FAIL;
    }
    h = bcd2hex(rtc_read_reg(HOUR_REG));
    if(h != bcd2hex(rtc_read_reg(HOUR_REG))){
        lprintf("read rtc reg 'hour' fail\n");
        return RTC_FAIL;
    }
    hb=h;mb=m;sb=s;
    if(scds>0){
        add_with_back_limit(&h,
                add_with_back_limit(&m,
                    add_with_back_limit(&s, scds, 60), 60),
                24);
        if(h<hb){
            lprintf("Hour exceeding upwards, adjust_time %d->%d fail\n", h, hb);
            return RTC_FAIL;
        }
    }
    else{
        scds=-scds;
        sub_with_back_limit(&h,
                sub_with_back_limit(&m,
                    sub_with_back_limit(&s, scds, 60), 60),
                24);
        if(h>hb){
            lprintf("Hour exceeding downwards, adjust_time %d->%d fail\n", h, hb);
            return RTC_FAIL;
        }
    }
    if(s>57){
        lprintf("Second %d near 59, adjust_time fail\n", h, hb);
        return RTC_FAIL;
    }
    if(RTC_FAIL==rtc_write_reg(SECOND_REG, hex2bcd(s))){
        lprintf("Second wt fail\n");
        return RTC_FAIL;
    }
    if(RTC_FAIL==rtc_write_reg(MINUTE_REG, hex2bcd(m))){
        lprintf("MIN wt fail\n");
        return RTC_FAIL;
    }
    if(RTC_FAIL==rtc_write_reg(HOUR_REG, hex2bcd(h))){
        lprintf("HOUR wt fail\n");
        return RTC_FAIL;
    }
    lprintf_time("%d:%d:%d %d->%d:%d:%d OK\n", hb, mb, sb, scds, h, m, s);
    return RTC_OK;
#else
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


void i2c_init()
{
    if(rtc_inited){
        lprintf("i2c inited already\n");
        return;
    }

    GPIO_InitTypeDef GPIO_InitStructure;       //GPIO

    RCC_APB2PeriphClockCmd(I2C_GPIO_PERIPH, ENABLE);

    GPIO_InitStructure.GPIO_Pin = SDA_PIN|SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C_GROUP, &GPIO_InitStructure);
    GPIO_SetBits(I2C_GROUP,SDA_PIN|SCL_PIN);
    rtc_inited = 1;
}

int min_zero()
{
    return 0==bcd2hex(rtc_read_reg(MINUTE_REG));
}


uint8_t pcf8574t_writeData(uchar mdata)
{
    uint8_t ret;
    Start();
    ret=writebyte(0x40); /*д����*/
    if(ret==0)goto err;
    ret=writebyte(mdata); /*д����*/
    Stop();
    return ret;
err:
    Stop();
    lprintf("pcf8574t writeData data %b error\n", mdata);
    return ret;
}
uint8_t pcf8574t_readData1(uchar * buff) /*���ֽ�*/
{  
    uint8_t ret;
    Start();
    ret=writebyte(0x41); /*������*/
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
