#ifndef _MONITOR_RE_H_
#define _MONITOR_RE_H_

#include "zf_common_headfile.h"
#include "headfile.h"

#define MONITOR_FILTER_MAX          (4)

extern int Diff_Data;
extern int16 Monitor_Data;

void my_ipc_callback(int receive_data);
void Monitor_ReRead(void);


#endif