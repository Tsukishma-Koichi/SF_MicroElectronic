#include "motor.h"

PID Motor1_PID;
PID Motor2_PID;
PID Motor1_D_PID;
PID Motor2_D_PID;
PID Motor_Diff_PID;
PD Motor_Diff_PD;

float Motor_target = 50;     //直线75
float Motor1_target = 0;   // 电机1PID目标（编码器读数）
float Motor2_target = 0;   // 电机2PID目标（编码器读数）


void Motor_Init(void)
{
    gpio_init(MOTOR_1_DIR, GPO, GPIO_HIGH, GPO_PUSH_PULL);                           // GPIO 初始化为输出 默认上拉输出高
    pwm_init(MOTOR_1_PWM, 17000, 0);                                                 // PWM 通道初始化频率 17KHz 占空比初始为 0
    gpio_init(MOTOR_2_DIR, GPO, GPIO_HIGH, GPO_PUSH_PULL);                           // GPIO 初始化为输出 默认上拉输出高
    pwm_init(MOTOR_2_PWM, 17000, 0);                                                 // PWM 通道初始化频率 17KHz 占空比初始为 0
    
}

/**
 * @brief 设置电机转速
 *
 * @param motor_name 所选择的电机
 * @param speed 所选择的速度
 *
 * @warning speed的范围是0-10000
 * @note Motor_SetSpeed(MOTOR_1, 1000)
 */

void Motor_SetSpeed(MOTOR_PWM_enum motor, int16 speed)
{
    if (motor == MOTOR_1)
    {
        if (speed < 0)
        {
            speed = -speed;
            gpio_set_level(MOTOR_1_DIR, 0);
            pwm_set_duty(MOTOR_1_PWM, speed);
        }
        else
        {
            gpio_set_level(MOTOR_1_DIR, 1);
            pwm_set_duty(MOTOR_1_PWM, speed);
        }
    }
    else if (motor == MOTOR_2)
    {
        if (speed < 0)
        {
            speed = -speed;
            gpio_set_level(MOTOR_2_DIR, 0);
            pwm_set_duty(MOTOR_2_PWM, speed);
        }
        else
        {
            gpio_set_level(MOTOR_2_DIR, 1);
            pwm_set_duty(MOTOR_2_PWM, speed);
        }
    }
}


void Motor_PID_Init(void)
{
    PID_Init(&Motor1_PID);
    PID_Init(&Motor2_PID);
//    PID_Init(&Motor_Diff_PID);
    PD_Init(&Motor_Diff_PD);
    
    Motor1_PID_Set(MOTOR_PID_P, MOTOR_PID_I, MOTOR_PID_D, MOTOR_PID_SL, MOTOR_PID_UL, 1);
    Motor2_PID_Set(MOTOR_PID_P, MOTOR_PID_I, MOTOR_PID_D, MOTOR_PID_SL, MOTOR_PID_UL, 1);
    
//    Motor_Diff_PID_Set(MOTOR_Diff_PID_P, MOTOR_Diff_PID_I, MOTOR_Diff_PID_D, MOTOR_Diff_PID_SL, MOTOR_Diff_PID_UL, 1);
    Motor_Diff_PD_Set(MOTOR_Diff_PD_P1, MOTOR_Diff_PD_P2, MOTOR_Diff_PD_D1, MOTOR_Diff_PD_D2, MOTOR_Diff_PD_UL, 1);
}

void Motor1_PID_Set(float K_p_set, float K_i_set, float K_d_set, float pLimit, float coLimit, float boost)
{
    PID_SetParameter(&Motor1_PID, K_p_set, K_i_set, K_d_set, pLimit, coLimit, boost);
}
void Motor2_PID_Set(float K_p_set, float K_i_set, float K_d_set, float pLimit, float coLimit, float boost)
{
    PID_SetParameter(&Motor2_PID, K_p_set, K_i_set, K_d_set, pLimit, coLimit, boost);
}


/**
 * @brief  差速
 *
 */
void Motor_Diff(void)
{
    if (Monitor_Data > MOTOR_Diff_threshold)
    {
//        PID_PostionalPID(&Motor_Diff_PID, 0, Monitor_Data);
        PID_2PD(&Motor_Diff_PD, 0, Monitor_Data);
        Motor1_target = Motor_target - Motor_Diff_PD.ut;
        Motor2_target = Motor_target + Motor_Diff_PD.ut;
    }
    else if (Monitor_Data < -MOTOR_Diff_threshold)
    {
//        PID_PostionalPID(&Motor_Diff_PID, 0, -Monitor_Data);
        PID_2PD(&Motor_Diff_PD, 0, -Monitor_Data);
        Motor1_target = Motor_target + Motor_Diff_PD.ut;
        Motor2_target = Motor_target - Motor_Diff_PD.ut;
    }
    else
    {
        Motor1_target = Motor_target;
        Motor2_target = Motor_target;
    }
    
}

void Motor_Diff_PID_Set(float K_p_set, float K_i_set, float K_d_set, float pLimit, float coLimit, float boost)
{
    PID_SetParameter(&Motor_Diff_PID, K_p_set, K_i_set, K_d_set, pLimit, coLimit, boost);
}

void Motor_Diff_PD_Set(float Kp1, float Kp2, float Kd1, float Kd2, float coLimit, float boost)
{
    PD_SetParameter(&Motor_Diff_PD, Kp1, Kp2, Kd1, Kd2, coLimit, boost);
}

/**
 * @brief 电机PID运行
 * 
 */
void Motor1_PIDwork(void)
{
    PID_IncrementalPID(&Motor1_PID, Motor1_target, Encoder_1Data);
    Motor_SetSpeed(MOTOR_1, (int16)Motor1_PID.ut);
}

void Motor2_PIDwork(void)
{
    PID_IncrementalPID(&Motor2_PID, Motor2_target, Encoder_2Data);
    Motor_SetSpeed(MOTOR_2, (int16)Motor2_PID.ut);
}
