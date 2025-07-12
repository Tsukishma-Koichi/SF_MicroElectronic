#ifndef _view_fun_h
#define _view_fun_h

#include "zf_common_headfile.h"

extern int m7_0_data[5];

// 巡线模式
#define NORMAL_MODE 0
#define LEFT_MODE 1
#define RIGHT_MODE 2

// 赛道元素
#define ROUNDA 1
#define BEND 2
#define SSZ 3
#define OR 4 
// 左右
#define LEFT 1 
#define RIGHT 2

struct STRUCT_ELEMENT_STATE {
    uint8 Round_Flag;
    uint8 Bend_Flag;
    uint8 SSZone_Flag;
    uint8 OR_Flag;
} ;

typedef struct STRUCT_ELEMENT_STATE ELEMENT_STATE;


struct STRUCT_ROUNDABOUT {
    uint8 R_Enter_Flag;
    uint8 L_Enter_Flag;
    uint8 R_In_Flag;
    uint8 L_In_Flag;
    uint8 R_Out_Flag;
    uint8 L_Out_Flag;
    uint8 Finish_Flag;
} ;

typedef struct STRUCT_ROUNDABOUT ROUND;


uint8 otsu_threshold(uint8 *image_data, uint32 width, uint32 height);
void image_binarize(uint8 *src, uint8 *dst, uint32 width, uint32 height, uint8 threshold);
void mean_filter(uint8 *src, uint8 *dst);
uint8 seek_white(uint8 mid, uint8 row, uint8 img[MT9V03X_H][MT9V03X_W]);
void mid_seek_boundary(uint8 img[MT9V03X_H][MT9V03X_W]);
void LR_seek_boundary(uint8 img[MT9V03X_H][MT9V03X_W]);
int get_offset(ELEMENT_STATE *E);

void State_Clear(ELEMENT_STATE *E);
uint8 State_Check(ELEMENT_STATE *E);

void Round_Clear(ROUND *Round);
void Roundabout_detect(uint8 img[MT9V03X_H][MT9V03X_W], ROUND *Round, ELEMENT_STATE *E);

void StartStopZone_detect(ELEMENT_STATE*E);
void Bend_detect(ELEMENT_STATE*E);
void DashedLine_detect(ELEMENT_STATE*E, uint8 img[MT9V03X_H][MT9V03X_W]);
void OpenRoad_detect(ELEMENT_STATE*E, uint8 img[MT9V03X_H][MT9V03X_W]);

void Roundabout_detect2(uint8 img[MT9V03X_H][MT9V03X_W], ROUND *Round, ELEMENT_STATE*E);

#endif











