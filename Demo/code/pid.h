#ifndef _PID_H_
#define _PID_H_

#include "zf_common_headfile.h"
#include "headfile.h"
#include <math.h>


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
        
//PID结构体
struct STRUCT_PD {
        float kP1;
        float kD1;
        float kP2;
        float kD2;
        float preError;
        float ppreError;
        float ut;
        float utLimit;
};

typedef struct STRUCT_PID PID;
void PID_Init(PID* p);
void PID_SetParameter(PID* p, float setP, float setI, float setD, float pLimit, float coLimit, float boost);
void PID_PostionalPID(PID* p, float target, float cur);
void PID_IncrementalPID(PID *p, float target, float cur);

typedef struct STRUCT_PD PD;
void PD_Init(PD *p);
void PID_2PD(PD *p, float target, float cur);
void PD_SetParameter(PD *p, float P1, float P2, float D1, float D2, float coLimit, float boost);


#endif