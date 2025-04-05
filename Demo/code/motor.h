#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "zf_common_headfile.h"
#include "headfile.h"
//      ģ��ܽ�            ��Ƭ���ܽ�
//      1DIR                P09_1 ���� P05_1
//      1PWM                P09_0 ���� P05_0
//      GND                 GND
//      2DIR                P10_3 ���� P05_3
//      2PWM                P10_2 ���� P05_2
//      GND                 GND
//      ���߶��� +          �������
//      ���߶��� -          ��ظ���

//#define DUTY_MAX            (4000)                                               // ��� MAX_DUTY% ռ�ձ�
//#define DUTY_LIM            (10000)

#define MOTOR_1_DIR              (P09_1)
#define MOTOR_1_PWM              (TCPWM_CH24_P09_0)

#define MOTOR_2_DIR              (P10_3)
#define MOTOR_2_PWM              (TCPWM_CH30_P10_2)

extern float Motor_target;
extern float Motor1_target;   // ���1PIDĿ�꣨������������
extern float Motor2_target;   // ���2PIDĿ�꣨������������

//extern PID Motor_Diff_PID;

typedef enum
{
    MOTOR_1 = 0x00,
    MOTOR_2 = 0x01
} MOTOR_PWM_enum;

void Motor_Init(void);
//void Motor_L(uint8 dir, uint16 duty);
//void Motor_R(uint8 dir, uint16 duty);
void Motor_SetSpeed(MOTOR_PWM_enum motor, int16 speed);

// PID���� λ��ʽ
//#define MOTOR_PID_P             (60)
//#define MOTOR_PID_I             (0.5)
//#define MOTOR_PID_D             (-2)
//#define MOTOR_PID_SL            (2500)
//#define MOTOR_PID_UL            (3000)
//����ʽ 75
#define MOTOR_PID_P             (9.5)
#define MOTOR_PID_I             (1.2)
#define MOTOR_PID_D             (0)
#define MOTOR_PID_SL            (2000)
#define MOTOR_PID_UL            (4500)



//����PID λ��ʽ
#define MOTOR_Diff_PID_P             (2)
#define MOTOR_Diff_PID_I             (0.8)
#define MOTOR_Diff_PID_D             (300)
#define MOTOR_Diff_PID_SL            (40)
#define MOTOR_Diff_PID_UL            (150)
//����˫PD
#define MOTOR_Diff_PD_P1             (0.12)
#define MOTOR_Diff_PD_P2             (0)
#define MOTOR_Diff_PD_D1             (10)
#define MOTOR_Diff_PD_D2             (0)
#define MOTOR_Diff_PD_UL             (350)

#define MOTOR_Diff_threshold            (5)



extern float Motor_target;

void Motor_PID_Init(void);
void Motor1_PID_Set(float K_p_set, float K_i_set, float K_d_set, float pLimit, float coLimit, float boost);
void Motor2_PID_Set(float K_p_set, float K_i_set, float K_d_set, float pLimit, float coLimit, float boost);

void Motor_pidClear(void);

//void Motor_GetTarget(void);
void Motor_Diff(void);
void Motor_Diff_PID_Set(float K_p_set, float K_i_set, float K_d_set, float pLimit, float coLimit, float boost);
void Motor_Diff_PD_Set(float Kp1, float Kp2, float Kd1, float Kd2, float coLimit, float boost);
void Motor1_PIDwork(void);
void Motor2_PIDwork(void);


#endif