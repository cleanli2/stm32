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

void disp(uint32_t w, uint8_t *cs, uint32_t len)
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

void main()
{
    printf("helloW\n");
    disp(1,chs,2);
    printf("\n\n\n\n\n");
    disp(2,ziku,32);
    printf("\n\n\n\n\n");
    disp(2,ziku12,24);
    for(int i = 0;i<20;i++){
    printf("\n\n\n\n\n");
    disp(2,ziku12+24*i,24);
    }
}
