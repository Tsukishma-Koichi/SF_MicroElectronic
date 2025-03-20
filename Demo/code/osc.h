#ifndef _OSC_H_
#define _OSC_H_

#include "zf_common_headfile.h"

extern seekfree_assistant_oscilloscope_struct oscilloscope_data;

void OSC_Init(void);
void OSC_Send(uint16 target, uint16 Data);


#endif