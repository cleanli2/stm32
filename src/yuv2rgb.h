#ifndef __YUV2RGB_
#define __YUV2RGB_

struct dataof_yuv2bmp {
	unsigned char y1;
	unsigned char y2;
	unsigned char u;
	unsigned char v;
	unsigned char b1;
	unsigned char g1;
	unsigned char r1;
	unsigned char b2;
	unsigned char g2;
	unsigned char r2;
};
#define CLAMP(L, D, H) ((D<L)?L:((D<H)?D:H))
static inline void yuv2bmp(struct dataof_yuv2bmp* dy)
{
	int y1, y2, u, v;
	int r1, r2, b1, b2, g1, g2;
	y1=dy->y1;
	u=dy->u;
	y2=dy->y2;
	v=dy->v;

    //compute
    b1=y1+1770*(u-128)/1000;
    g1=y1-(343*(u-128)+714*(v-128))/1000;
    r1=y1+1403*(v-128)/1000;
    b2=y2+1770*(u-128)/1000;
    g2=y2-(343*(u-128)+714*(v-128))/1000;
    r2=y2+1403*(v-128)/1000;

    //output
    dy->b1=CLAMP(0, b1, 255);
    dy->b2=CLAMP(0, b2, 255);
    dy->g1=CLAMP(0, g1, 255);
    dy->g2=CLAMP(0, g2, 255);
    dy->r1=CLAMP(0, r1, 255);
    dy->r2=CLAMP(0, r2, 255);
}

#endif
