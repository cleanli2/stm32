#ifndef __PYINPUT_H
#define __PYINPUT_H
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


//ƴ�������ƴ���Ķ�Ӧ��
typedef struct
{
  char *py_input;//������ַ���
  char *py;	   //��Ӧ��ƴ��
  int num;
  unsigned char *pymb_ch;
}py_index;

#define MAX_MATCH_PYMB 	10	//���ƥ����

#define MAX_PY_OPS 4

//ƴ�����뷨
typedef struct
{
  unsigned char(*getpymb)(unsigned char *instr);			//�ַ���������ȡ����
  py_index *pymb[MAX_MATCH_PYMB];	//�����λ��
  int mwdth;
}pyinput;

extern pyinput t9;
extern pyinput eng;
unsigned char str_match(unsigned char*str1,unsigned char*str2);
unsigned char get_matched_pymb(unsigned char *strin,py_index **matchlist);
unsigned char get_pymb(unsigned char* str);
void test_py(unsigned char *inputstr);
extern const py_index py_index3[];
extern const py_index cy_index[];
extern const py_index eng_mb_index[];
int size_of_pyindex();
int size_of_cyindex();
int size_of_engmbindex();
#endif
