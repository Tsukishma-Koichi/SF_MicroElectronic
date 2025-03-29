#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "zf_common_headfile.h"
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

//#define DUTY_MAX            (4000)                                               // 最大 MAX_DUTY% 占空比
//#define DUTY_LIM            (10000)

#define MOTOR_1_DIR              (P09_1)
#define MOTOR_1_PWM              (TCPWM_CH24_P09_0)

#define MOTOR_2_DIR              (P10_3)
#define MOTOR_2_PWM              (TCPWM_CH30_P10_2)

extern float Motor_target;
extern float Motor1_target;   // 电机1PID目标（编码器读数）
extern float Motor2_target;   // 电机2PID目标（编码器读数）

typedef enum
{
    MOTOR_1 = 0x00,
    MOTOR_2 = 0x01
} MOTOR_PWM_enum;

void Motor_Init(void);
//void Motor_L(uint8 dir, uint16 duty);
//void Motor_R(uint8 dir, uint16 duty);
void Motor_SetSpeed(MOTOR_PWM_enum motor, int16 speed);

// PID参数 位置式
//#define MOTOR_PID_P             (60)
//#define MOTOR_PID_I             (0.5)
//#define MOTOR_PID_D             (-2)
//#define MOTOR_PID_SL            (2500)
//#define MOTOR_PID_UL            (3000)
//增量式
#define MOTOR_PID_P             (9.5)
#define MOTOR_PID_I             (1.2)
#define MOTOR_PID_D             (0)
#define MOTOR_PID_SL            (2000)
#define MOTOR_PID_UL            (2500)


//// 差速相关
//#define MOTOR_DIFSPEED_FACTOR   (0.25)   // 差速修正系数
//#define MOTOR_DIFSPEED_THRE     (45)    // 差速触发死区（摄像头偏航角）
//#define Diff_MAX                (90)

//差速PID
#define MOTOR_Diff_PID_P             (9.5)
#define MOTOR_Diff_PID_I             (1.2)
#define MOTOR_Diff_PID_D             (0)
#define MOTOR_Diff_PID_SL            (20)
#define MOTOR_Diff_PID_UL            (35)

extern float Motor_target;

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

//void Motor_GetTarget(void);
void Motor_Diff(void);
void Motor_Diff_PID_Set(float K_p_set, float K_i_set, float K_d_set, float pLimit, float coLimit, float boost);


void Motor1_PIDwork(void);
void Motor2_PIDwork(void);


#endif