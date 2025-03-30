#ifndef _OSC_H_
#define _OSC_H_

#include "zf_common_headfile.h"

extern seekfree_assistant_oscilloscope_struct oscilloscope_data;

void OSC_Init(void);
void OSC_Send(int Data0, int Data1, int Data2, int Data3, int Data4, int Data5, int Data6, int Data7);


#endif