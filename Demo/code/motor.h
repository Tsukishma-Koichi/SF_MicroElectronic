#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "zf_common_headfile.h"
#include "encoder.h"
#include "headfile.h"
//      模块管脚            单片机管脚
//      1DIR                P09_1 或者 P05_1
//      1PWM                P09_0 或者 P05_0
//      GND                 GND
//      2DIR                P10_3 或者 P05_3
//      2PWM                P10_2 或者 P05_2
//      GND                 GND
//      接线端子 +          电池正极
//      接线端子 -          电池负极

#define DUTY_MAX            (4000)                                               // 最大 MAX_DUTY% 占空比
#define DUTY_LIM            (10000)

#define DIR_L              (P09_1)
#define PWM_L              (TCPWM_CH24_P09_0)

#define DIR_R              (P10_3)
#define PWM_R              (TCPWM_CH30_P10_2)

void Motor_Init(void);
void Motor_L(uint8 dir, uint16 duty);
void Motor_R(uint8 dir, uint16 duty);

// PID参数
#define MOTOR_PID_P             (12)
#define MOTOR_PID_I             (0.8)
#define MOTOR_PID_D             (-1)
#define MOTOR_PID_SL            (2500)
#define MOTOR_PID_UL            (4000)

void Motor_PID_Init(void);
void Motor1_PID_Set(float K_p_set, float K_i_set, float K_d_set, float pLimit, float coLimit, float boost);
void Motor2_PID_Set(float K_p_set, float K_i_set, float K_d_set, float pLimit, float coLimit, float boost);
void Motor1_SetPIDP(float setP);
void Motor1_SetPIDI(float setI);
void Motor1_SetPIDD(float setD);
void Motor1_SetPIDSumLimit(float pLimit);
void Motor1_SetPIDCoLimit(float coLimt);
void Motor2_SetPIDP(float setP);
void Motor2_SetPIDI(float setI);
void Motor2_SetPIDD(float setD);
void Motor2_SetPIDSumLimit(float pLimit);
void Motor2_SetPIDCoLimit(float coLimt);
void Motor_pidClear(void);

void MotorR_PIDwork(float MotorR_target);
void MotorL_PIDwork(float MotorL_target);


#endif