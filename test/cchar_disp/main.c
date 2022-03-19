#include<stdio.h>
#include<stdint.h>
uint8_t chs[]={0x55,0x99};
extern const uint8_t ziku[] ;
extern const uint8_t ziku12[] ;
void dispbyte(uint8_t b)
{
    int i = 8;
    while(i--){
        if(b&0x80){
            printf("*");
        }
        else{
            printf(" ");
        }
        b<<=1;
    }
}

void disp(uint32_t w, const uint8_t *cs, uint32_t len)
{
    int i = 0;
    while(len--){
        dispbyte(*cs++);
        if(w == 1)printf("\n");
        if(i & 1 && w == 2){
            printf("\n");
        }
        i++;
    }
}

void disp1cch16(uint32_t w, const uint8_t *cs)
{
    //Hp=((GBH-0xA1)×94+GBL-0XA1)×(size);
    int offset = ((cs[0]-0xa1)*94+cs[1]-0xa1)*32;
    disp(w,ziku+offset,32);
}

void disp_cch16_str(uint32_t w, const uint8_t *cs)
{
    while(*cs){
        if(*cs<0x80){
            cs++;
            continue;
        }
        disp1cch16(w, cs);
        cs+=2;
    }
}

extern const uint8_t* cch_test;
extern const uint8_t* cch_test2;
const char* cch_test_str;
void main()
{
    printf("helloW\n");
    disp(1,chs,2);
    printf("\n\n\n\n\n");
    printf("%02x %02x\n", cch_test[0], cch_test[1]);
    disp1cch16(2, cch_test);
    printf("%02x %02x\n", cch_test2[0], cch_test2[1]);
    disp1cch16(2, cch_test2);
    disp_cch16_str(2, cch_test_str);
    disp(2,ziku,32);
    printf("\n\n\n\n\n");
    disp(2,ziku12,24);
    /*
    for(int i = 0;i<20;i++){
    printf("\n\n\n\n\n");
    disp(2,ziku12+24*i,24);
    }
    */
}
