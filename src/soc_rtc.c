#include "sys.h"
#include "soc_rtc.h" 		    
	   
date_time g_dt={0,0,0,25,10,14,2};
date_time *dt=&g_dt;
char datestr[20]={0};
//if Leap year
//month   1  2  3  4  5  6  7  8  9  10 11 12
//leap    31 29 31 30 31 30 31 31 30 31 30 31
//no leap 31 28 31 30 31 30 31 31 30 31 30 31
//in: year
//out:1,yes.0,no
u8 Is_Leap_Year(u16 year)
{			  
	if(year%4==0)
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0)return 1;
			else return 0;   
		}else return 1;   
	}else return 0;	
}	 			   
//1970.1.1
//1970~2099 is valid
//:0,sucess;other fail
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5};
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
u32 date2seconds()
{
	u16 t;
	u32 seccount=0;
	if(dt->w_year<1970||dt->w_year>2099)return 0;	   
	for(t=1970;t<dt->w_year;t++)
	{
		if(Is_Leap_Year(t))seccount+=31622400;
		else seccount+=31536000;
	}
	for(t=0;t<(dt->w_month-1);t++)//add seconds of months
	{
		seccount+=(u32)mon_table[t]*86400;
		if(Is_Leap_Year(dt->w_year)&&t==1)seccount+=86400;//one day more if Leap year
	}
	seccount+=(u32)(dt->w_date-1)*86400;//days seconds
	seccount+=(u32)dt->hour*3600;//hours seconds
    seccount+=(u32)dt->min*60;// minutes seconds
	seccount+=dt->sec;
    return seccount;
}

void RTC_Set()
{
	u32 seccount=date2seconds(dt);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	RTC_SetCounter(seccount);

	RTC_WaitForLastTask();
}

//only 1901-2099 allowed
u8 date2week(u16 year,u8 month,u8 day)
{	
	u16 temp2;
	u8 yearH,yearL;
	
	yearH=year/100;	yearL=year%100; 
	//21centry,plus 100  
	if (yearH>19)yearL+=100;
	//only compute after 1900
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}			  

void seconds2date(u32 timecount)
{
	u32 temp=0;
	u16 temp1=0;	  

 	temp=timecount/86400;//days

    temp1=1970;
    while(temp>=365)
    {				 
        if(Is_Leap_Year(temp1))
        {
            if(temp>=366)temp-=366;
            else {temp1++;break;}  
        }
        else temp-=365;
        temp1++;  
    }   
    dt->w_year=temp1;//get year
    temp1=0;
    while(temp>=28)//more than one month
    {
        if(Is_Leap_Year(dt->w_year)&&temp1==1)//is leap year/Feb?
        {
            if(temp>=29)temp-=29;
            else break; 
        }
        else 
        {
            if(temp>=mon_table[temp1])temp-=mon_table[temp1];//not leap year
            else break;
        }
        temp1++;  
    }
    dt->w_month=temp1+1;
    dt->w_date=temp+1;
    temp=timecount%86400;
	dt->hour=temp/3600;
	dt->min=(temp%3600)/60;
	dt->sec=(temp%3600)%60;
	dt->week=date2week(dt->w_year,dt->w_month,dt->w_date);
}
char*RTC_Get(void)
{
	u32 timecount=0; 
    timecount=RTC_GetCounter();	 
    seconds2date(timecount);
    slprintf(datestr, "%d.%d.%d-%d:%d:%d\n", dt->w_year, dt->w_month, dt->w_date,
            dt->hour, dt->min, dt->sec);
    return datestr;
}	 
