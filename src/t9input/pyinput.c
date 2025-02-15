#include "pyinput.h"
#include "string.h"
#include "common.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ο������粢�����޸�
//ALIENTEKս��STM32������
//ƴ�����뷨 ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/22
//�汾��V1.0			    
//������������ӿƼ����޹�˾  	 												    								  
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
	return mcnt;//����ƥ��ĸ���
}

unsigned char get_eng_mb(unsigned char* str)
{
	return get_matched_mb(str,eng.pymb);
}

//ƴ�����뷨
pyinput t9=
{
	get_pymb,
    {0},
    2,
};

//�Ƚ������ַ�����ƥ�����
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

#define MOST_MB_SIZE 241
#define DYN_HISTORY_SIZE 211
#define DYN_MB_SIZE 241
int dyn_his_p=0;
const unsigned char const_dyn_mb[DYN_MB_SIZE+1]={"���ǲ���һ�������и����������ܹ�ô������Ҳ�Ƕ����Ǵ�����ȥҪ����Ϊ�ɿ���˵ʲ����ʱ��������廹�ø߳��������ּ���û�����������µö��β������Щ����������ŵ������������򲡺�����֮���ɷ��������ʦѧ���غ�����ֻ���ʿ���ʵ���Զ���������"};
unsigned char dyn_mb[DYN_MB_SIZE+1]={0};
unsigned char         most_mb[MOST_MB_SIZE+1] = {"�ʵ���̽о�Ȼ����Ӿ���������Ŀ������С�ɴ���֪ͬ�������ŵ�����Խ�߳Է����������������ѳ�����˭���ߴ���Ŵ���ʶȫ�ȵ�ҵ��̬˼ͷ��ǰ�ε����Ļ���Ԫת��ر߸��ޱȵ��Ѷ������������ֱ��״���ֲ�ͨʽִ�������ְ����ع����������������̫�������"};
py_index const_dyn_index={"dynch","words",2,DYN_MB_SIZE,(unsigned char*)const_dyn_mb};
py_index dyn_index={"dynch","words",2,DYN_MB_SIZE,dyn_mb};
py_index most_index={"dynch","words",2,DYN_MB_SIZE,most_mb};
unsigned char dyn_his_mb[DYN_HISTORY_SIZE+1]={0};
py_index dyn_his_index={"history","histywds",2,DYN_HISTORY_SIZE,dyn_his_mb};

void str_leftmove(char*s, int n);
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
    pyindex_len=size_of_pyindex();//�õ�py������Ĵ�С.
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

//��ȡƥ���ƴ�����
//*strin,������ַ���,����:"726"
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
        pyindex_len=size_of_pyindex();//�õ�py������Ĵ�С.
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
	return mcnt;//����ƥ��ĸ���
}

//�õ�ƴ�����.
//str:�����ַ���
//����ֵ:ƥ�����.
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
		//printf("\r\n��������Ϊ:%s\r\n",inputstr);
		//printf("����ƥ��λ��:%d\r\n",t&0X7F);
		//printf("����ƥ����:%s,%s\r\n",t9.pymb[0]->py,t9.pymb[0]->pymb);
	}else if(t)
	{
		//printf("\r\n��������Ϊ:%s\r\n",inputstr);
		//printf("��ȫƥ�����:%d\r\n",t);
		//printf("��ȫƥ��Ľ��:\r\n");
		for(i=0;i<t;i++)
		{
			//printf("%s,%s\r\n",t9.pymb[i]->py,t9.pymb[i]->pymb);
		}
	}
    //else printf("û���κ�ƥ������\r\n");
}
