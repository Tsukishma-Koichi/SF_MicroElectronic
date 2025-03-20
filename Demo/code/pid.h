#ifndef _PID_H_
#define _PID_H_

#include "zf_common_headfile.h"
#include "headfile.h"

//uint16 PID_velo1(int reality, int target);
//uint16 PID_velo2(int reality, int target);
//

//PID类型
typedef enum{
    PostionalPID,IncrementalPID
}PIDCLASS_enum;
//PID结构体
struct STRUCT_PID {
        float kP;
        float kI;
        float kD;
        float kPSet;
        float kISet;
        float kDSet;
        float sum;
        float preError;
        float ppreError;
        float ut;
        float sumLimit;
        float utLimit;
};

typedef struct STRUCT_PID PID;
void PID_Init(PID* p);
void PID_SetParameter(PID* p, float setP, float setI, float setD, float pLimit, float coLimit, float boost);
void PID_PostionalPID(PID* p, float target, float cur);


#endif