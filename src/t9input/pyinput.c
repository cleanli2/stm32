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

#define DYN_HISTORY_SIZE 40
int dyn_his_p=0;
unsigned char dyn_mb[73]={"��һ�ǲ������������������������ʱ���Ϊ������˵���ž���ҪҲ��������Ի�"};
py_index dyn_index={"dynch","words",73,dyn_mb};
unsigned char dyn_his_mb[DYN_HISTORY_SIZE+1]={"��һ�ǲ������������������������ʱ���Ϊ"};
py_index dyn_his_index={"history","histywds",40,dyn_his_mb};

int chs_is_in_list(char*ch, char*list, int len)
{
    for(int i=0;i<len;i+=2){
        if((ch[0]==list[i])&&(ch[1]==list[i+1])){
            return 1;
        }
    }
    return 0;
}

void chs_put_in_list(char*ch, char*list)
{
    list[dyn_his_p]=ch[0];
    list[dyn_his_p+1]=ch[1];
    dyn_his_p+=2;
    if(dyn_his_p>=DYN_HISTORY_SIZE){
        dyn_his_p=0;
    }
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
    else if(strin[0]<=0x80){
        pyindex_len=size_of_pyindex();//�õ�py������Ĵ�С.
        for(i=0;i<pyindex_len;i++)
        {
            temp=str_match(strin,(unsigned char*)py_index3[i].py_input);
            if(temp > 0 && ((mcnt<MAX_PY_OPS)||(0x80&temp)))
            {
                matchlist[mcnt++]=(py_index*)&py_index3[i];
                t9.mwdth=2;
            }
        }
        if(mcnt==0){
            pyindex_len=size_of_cyindex();
            for(i=0;i<pyindex_len;i++)
            {
                temp=str_match(strin,(unsigned char*)cy_index[i].py_input);
                if(temp > 0 && ((mcnt<MAX_PY_OPS)||(0x80&temp)))
                {
                    matchlist[mcnt++]=(py_index*)&cy_index[i];
                    t9.mwdth=4;
                }
            }
        }
    }
    else{
        if(!chs_is_in_list((char*)strin, (char*)dyn_his_mb, DYN_HISTORY_SIZE)){
            chs_put_in_list((char*)strin, (char*)dyn_his_mb);
        }
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
