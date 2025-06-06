#ifndef __PYINPUT_H
#define __PYINPUT_H
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
#include <stdint.h>


//拼音码表与拼音的对应表
typedef struct
{
  char *py_input;//输入的字符串
  char *py;	   //对应的拼音
  uint16_t grp_n;
  uint16_t num;
  unsigned char *pymb_ch;
}py_index;

#define MAX_MATCH_PYMB 	10	//最大匹配数

#define MAX_PY_OPS 4

//拼音输入法
typedef struct
{
  unsigned char(*getpymb)(unsigned char *instr);			//字符串到码表获取函数
  py_index *pymb[MAX_MATCH_PYMB];	//码表存放位置
  int mwdth;
}pyinput;

extern pyinput t9;
extern pyinput eng;
extern const char htcdm[];
extern const char htmm[];
#define MOST_MB_SIZE 241
#define DYN_HISTORY_SIZE 211
#define DYN_MB_SIZE 241
int find_wd(const char*list, char*ch);
unsigned char str_match(unsigned char*str1,unsigned char*str2);
unsigned char get_matched_pymb(unsigned char *strin,py_index **matchlist);
unsigned char get_pymb(unsigned char* str);
void test_py(unsigned char *inputstr);
void put_his_buf(unsigned char *strin);
extern const py_index py_index3[];
extern const py_index cy_index[];
extern const py_index eng_mb_index[];
extern const char ciyu2_list[][4];
int size_of_pyindex();
int size_of_cyindex();
int size_of_ciyu2_list();
int size_of_engmbindex();
#endif
