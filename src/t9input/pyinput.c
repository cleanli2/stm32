#include "pyinput.h"
#include "string.h"
#include "common.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序参考自网络并加以修改
//ALIENTEK战舰STM32开发板
//拼音输入法 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/22
//版本：V1.0			    
//广州市星翼电子科技有限公司  	 												    								  
//////////////////////////////////////////////////////////////////////////////////
unsigned char get_eng_mb(unsigned char* str);

pyinput eng=
{
	get_eng_mb,
    {0},
    1,
};

unsigned char get_matched_mb(unsigned char *strin,py_index **matchlist)
{
	int mbindex_len;
	int i;
	unsigned char temp,mcnt=0;
	mbindex_len=size_of_engmbindex();//
	for(i=0;i<mbindex_len;i++)
	{
		temp=str_match(strin,(unsigned char*)eng_mb_index[i].py_input);
		if(temp > 0 && ((mcnt<MAX_PY_OPS)||(0x80&temp)))
		{
			matchlist[mcnt++]=(py_index*)&eng_mb_index[i];
		}
	}
	return mcnt;//返回匹配的个数
}

unsigned char get_eng_mb(unsigned char* str)
{
	return get_matched_mb(str,eng.pymb);
}

//拼音输入法
pyinput t9=
{
	get_pymb,
    {0},
    2,
};

//比较两个字符串的匹配情况
unsigned char str_match(unsigned char*str1,unsigned char*str2)
{
	unsigned char i=0;
	while(1)
	{
		if(*str1=='\0'){
            if(*str2=='\0')return 0x80+i;
            else return i;
		}
		if(*str1!=*str2){
			return 0;
		}
		i++;
		str1++;
		str2++;
	}
}

int dyn_his_p=0;
const unsigned char const_dyn_mb[DYN_MB_SIZE+1]={"的是不人一这了你有个就在他我能功么来修炼也那都到们大法上中去要出它为可看讲说什以心时会多样种体还好高常想气所现家下没很身自西过事得东次层生真道些间给把正里着当佛子做己天因病后往性之开成发物用情候师学本呢和起化作只其问空许够实理别对而动题怎定"};
unsigned char dyn_mb[DYN_MB_SIZE+1]={0};
unsigned char         most_mb[MOST_MB_SIZE+1] = {"质点意教叫觉然宇宙从经象吗神行目但外形小干传求同知根坏特门地年命越走吃方于如变练老最存面难长量认谁轮者打相才带力识全度德业提态思头果前治掉念哪话社元转悟回边各无比等已儿受再世类界眼直信状代又部通式执感另让手白明关管完少整苦程著两放太达国主利"};
py_index const_dyn_index={"dynch","words",2,DYN_MB_SIZE,(unsigned char*)const_dyn_mb};
py_index dyn_index={"dynch","words",2,DYN_MB_SIZE,dyn_mb};
py_index most_index={"dynch","words",2,DYN_MB_SIZE,most_mb};
unsigned char dyn_his_mb[DYN_HISTORY_SIZE+1]={0};
py_index dyn_his_index={"history","histywds",2,DYN_HISTORY_SIZE+1,dyn_his_mb};

const char htcdm[]={"这个字在'M'组第%d个"};
const char htmm[]={"这个字在'm'组第%d个"};
void str_leftmove(char*s, int n);
int find_wd(const char*list, char*ch)
{
    int l=strlen(list);
    for(int i=0;i<l;i+=2){
        if((ch[0]==list[i])&&(ch[1]==list[i+1])){
            return i/2;
        }
    }
    return -1;
}
int chs_is_in_list(char*ch, char*list, int len)
{
    for(int i=0;i<len;i+=2){
        if((ch[0]==list[i])&&(ch[1]==list[i+1])){
            str_leftmove(list+i, 2);
            list[dyn_his_p-2]=ch[0];
            list[dyn_his_p-1]=ch[1];
            return 1;
        }
    }
    return 0;
}

void chs_put_in_list(char*ch, char*list)
{
    if(dyn_his_p>=DYN_HISTORY_SIZE){
        while(dyn_his_p>=DYN_HISTORY_SIZE){
            str_leftmove(list, 2);
            dyn_his_p-=2;
        }
    }
    list[dyn_his_p]=ch[0];
    list[dyn_his_p+1]=ch[1];
    dyn_his_p+=2;
}

void update_dyn(unsigned char*s)
{
	int pyindex_len, listlen;
	int i, j, dynp=DYN_MB_SIZE-2;
    pyindex_len=size_of_pyindex();//得到py索引表的大小.
    memcpy(dyn_mb, const_dyn_mb, DYN_MB_SIZE);
    for(i=0;i<pyindex_len;i++)
    {
        if(py_index3[i].grp_n<=2)continue;
        for(j=0;j<py_index3[i].num;j+=2){
            if(s[0]==py_index3[i].pymb_ch[j] && s[1]==py_index3[i].pymb_ch[j+1]
                    && (0!=(j+2)%py_index3[i].grp_n)){
                if(dynp<1)break;
                dyn_mb[dynp-1]=py_index3[i].pymb_ch[j+2];
                dyn_mb[dynp]=py_index3[i].pymb_ch[j+3];
                dynp-=2;
            }
        }
    }
    listlen=size_of_ciyu2_list();
    for(i=0;i<listlen;i++)
    {
        if(s[0]==ciyu2_list[i][0] && s[1]==ciyu2_list[i][1]){
            if(dynp<1)break;
            dyn_mb[dynp-1]=ciyu2_list[i][2];
            dyn_mb[dynp]=ciyu2_list[i][3];
            dynp-=2;
        }
    }
    //mem_print((char*)dyn_mb, 0, DYN_MB_SIZE);
}

void put_his_buf(unsigned char *strin)
{
    //lprintf("++dyn_his_p=%d\r\n", dyn_his_p);
    //mem_print(dyn_his_mb,0, 30);
    while(*strin){
        if(!(strin[0]==0xa1&&strin[1]==0xfd)){
            if(!chs_is_in_list((char*)strin, (char*)dyn_his_mb, DYN_HISTORY_SIZE)){
                chs_put_in_list((char*)strin, (char*)dyn_his_mb);
            }
        }
        strin+=2;
    }
    //lprintf("--dyn_his_p=%d\r\n", dyn_his_p);
    //mem_print(dyn_his_mb,0, 30);
}

//获取匹配的拼音码表
//*strin,输入的字符串,形如:"726"
unsigned char get_matched_pymb(unsigned char *strin,py_index **matchlist)
{
	int pyindex_len;
	int i;
	unsigned char temp,mcnt=0;
    if(strin[0]=='h'){
        matchlist[mcnt++]=&dyn_his_index;
    }
    else if(strin[0]=='m'){
        matchlist[mcnt++]=&most_index;
    }
    else if(strin[0]=='M'){
        matchlist[mcnt++]=&const_dyn_index;
    }
    else if(strin[0]<=0x80){
        pyindex_len=size_of_pyindex();//得到py索引表的大小.
        for(i=0;i<pyindex_len;i++)
        {
            temp=str_match(strin,(unsigned char*)py_index3[i].py_input);
            if(temp > 0 && ((mcnt<MAX_PY_OPS)||(0x80&temp)))
            {
                matchlist[mcnt++]=(py_index*)&py_index3[i];
                t9.mwdth=py_index3[i].grp_n;
            }
        }
    }
    else{
        update_dyn(strin);
        matchlist[mcnt++]=&dyn_index;
    }
	return mcnt;//返回匹配的个数
}

//得到拼音码表.
//str:输入字符串
//返回值:匹配个数.
unsigned char get_pymb(unsigned char* str)
{
	return get_matched_pymb(str,t9.pymb);
}

void test_py(unsigned char *inputstr)
{
	unsigned char t=0;
	unsigned char i=0;
	t=t9.getpymb(inputstr);
	if(t&0X80)
	{
		//printf("\r\n输入数字为:%s\r\n",inputstr);
		//printf("部分匹配位数:%d\r\n",t&0X7F);
		//printf("部分匹配结果:%s,%s\r\n",t9.pymb[0]->py,t9.pymb[0]->pymb);
	}else if(t)
	{
		//printf("\r\n输入数字为:%s\r\n",inputstr);
		//printf("完全匹配个数:%d\r\n",t);
		//printf("完全匹配的结果:\r\n");
		for(i=0;i<t;i++)
		{
			//printf("%s,%s\r\n",t9.pymb[i]->py,t9.pymb[i]->pymb);
		}
	}
    //else printf("没有任何匹配结果！\r\n");
}
