#ifndef _view_fun_h
#define _view_fun_h

#include "zf_common_headfile.h"

#endif

uint8 otsu_threshold(uint8 *image_data, uint32 width, uint32 height);
void image_binarize(uint8 *src, uint8 *dst, uint32 width, uint32 height, uint8 threshold);
void mean_filter(uint8 *src, uint8 *dst);
int seek_boundary(uint8 img[MT9V03X_H][MT9V03X_W]);

uint8 Roundabout_detect();
uint8 StartStopZone_detect();
uint8 Bend_detect();
uint8 DashedLine_detect();
uint8 OpenRoad_detect();