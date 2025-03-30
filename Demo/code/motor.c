#include "motor.h"

PID Motor1_PID;
PID Motor2_PID;
PID Motor1_D_PID;
PID Motor2_D_PID;
PID Motor_Diff_PID;

float Motor_target = 75;     //直线75
float Motor1_target = 0;   // 电机1PID目标（编码器读数）
float Motor2_target = 0;   // 电机2PID目标（编码器读数）

float Motor_D_target = 20;


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
    PID_Init(&Motor_Diff_PID);
    PID_Init(&Motor1_D_PID);
    
    Motor1_PID_Set(MOTOR_PID_P, MOTOR_PID_I, MOTOR_PID_D, MOTOR_PID_SL, MOTOR_PID_UL, 1);
    Motor2_PID_Set(MOTOR_PID_P, MOTOR_PID_I, MOTOR_PID_D, MOTOR_PID_SL, MOTOR_PID_UL, 1);
    
    Motor_Diff_PID_Set(MOTOR_Diff_PID_P, MOTOR_Diff_PID_I, MOTOR_Diff_PID_D, MOTOR_Diff_PID_SL, MOTOR_Diff_PID_UL, 1);
    Motor1_D_PID_Set(MOTOR_D_PID_P, MOTOR_D_PID_I, MOTOR_D_PID_D, MOTOR_D_PID_SL, MOTOR_D_PID_UL, 1);
    Motor2_D_PID_Set(MOTOR_D_PID_P, MOTOR_D_PID_I, MOTOR_D_PID_D, MOTOR_D_PID_SL, MOTOR_D_PID_UL, 1);
}

void Motor1_PID_Set(float K_p_set, float K_i_set, float K_d_set, float pLimit, float coLimit, float boost)
{
    PID_SetParameter(&Motor1_PID, K_p_set, K_i_set, K_d_set, pLimit, coLimit, boost);
}
void Motor2_PID_Set(float K_p_set, float K_i_set, float K_d_set, float pLimit, float coLimit, float boost)
{
    PID_SetParameter(&Motor2_PID, K_p_set, K_i_set, K_d_set, pLimit, coLimit, boost);
}

void Motor1_SetPIDP(float setP)
{
    Motor1_PID.kP = setP;
}

void Motor1_SetPIDI(float setI)
{
    Motor1_PID.kI = setI;
}

void Motor1_SetPIDD(float setD)
{

    Motor1_PID.kD = setD;
}

// 设置积分限制
void Motor1_SetPIDSumLimit(float pLimit)
{
    Motor1_PID.sumLimit = pLimit;
}

// 设置修正限幅
void Motor1_SetPIDCoLimit(float coLimt)
{
    Motor1_PID.utLimit = coLimt;
}

void Motor2_SetPIDP(float setP)
{

    Motor2_PID.kP = setP;
}

void Motor2_SetPIDI(float setI)
{
    Motor2_PID.kI = setI;
}

void Motor2_SetPIDD(float setD)
{
    Motor2_PID.kD = setD;
}
void Motor_pidClear()
{
    Motor1_PID.ut = 0.0;
    Motor2_PID.ut = 0.0;
}

// 设置积分限制
void Motor2_SetPIDSumLimit(float pLimit)
{
    Motor2_PID.sumLimit = pLimit;
}

// 设置修正限幅
void Motor2_SetPIDCoLimit(float coLimt)
{
    Motor2_PID.utLimit = coLimt;
}

///**
// * @brief 获取差速目标
// * 
// */
//void Motor_GetTarget(void)
//{
//    if (Diff_Data >= MOTOR_DIFSPEED_THRE)
//    {
//        Motor2_target = Motor_target;
//        Motor1_target = Motor_target * (1 - MOTOR_DIFSPEED_FACTOR * (Diff_Data) / Diff_MAX);
//    }
//    else if (-Diff_Data >= MOTOR_DIFSPEED_THRE)
//    {
//        Motor1_target = Motor_target;
//        Motor2_target = Motor_target * (1 - MOTOR_DIFSPEED_FACTOR * (-Diff_Data) / Diff_MAX);
//    }
//    else 
//    {
//        Motor1_target = Motor_target;
//        Motor2_target = Motor_target;
//    }
//}


/**
 * @brief  差速
 *
 */
void Motor_Diff(void)
{
    if (Monitor_Data > MOTOR_Diff_threshold)
    {
        PID_PostionalPID(&Motor_Diff_PID, 0, Monitor_Data);
        Motor1_target = Motor_target - Motor_Diff_PID.ut;
        Motor2_target = Motor_target + Motor_Diff_PID.ut;
    }
    else if (Monitor_Data < -MOTOR_Diff_threshold)
    {
        PID_PostionalPID(&Motor_Diff_PID, 0, -Monitor_Data);
        Motor1_target = Motor_target + Motor_Diff_PID.ut;
        Motor2_target = Motor_target - Motor_Diff_PID.ut;
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

void Motor1_D_PID_Set(float K_p_set, float K_i_set, float K_d_set, float pLimit, float coLimit, float boost)
{
    PID_SetParameter(&Motor1_D_PID, K_p_set, K_i_set, K_d_set, pLimit, coLimit, boost);
}

void Motor2_D_PID_Set(float K_p_set, float K_i_set, float K_d_set, float pLimit, float coLimit, float boost)
{
    PID_SetParameter(&Motor2_D_PID, K_p_set, K_i_set, K_d_set, pLimit, coLimit, boost);
}

/**
 * @brief 电机PID运行
 * 
 */
void Motor1_PIDwork(void)
{
//    PID_PostionalPID(&Motor1_PID, Motor1_target, Encoder_1Data);
    PID_IncrementalPID(&Motor1_PID, Motor1_target, Encoder_1Data);
//    PID_IncrementalPID(&Motor1_D_PID, Motor_D_target, Encoder_1Data);
    Motor_SetSpeed(MOTOR_1, (int16)Motor1_PID.ut);
//    Motor_SetSpeed(MOTOR_1, (int16)Motor1_D_PID.ut);
}

void Motor2_PIDwork(void)
{
//    PID_PostionalPID(&Motor2_PID, Motor2_target, Encoder_2Data);
    PID_IncrementalPID(&Motor2_PID, Motor2_target, Encoder_2Data);
//    PID_IncrementalPID(&Motor2_D_PID, Motor_D_target, Encoder_2Data);
    Motor_SetSpeed(MOTOR_2, (int16)Motor2_PID.ut);
//    Motor_SetSpeed(MOTOR_2, (int16)Motor2_D_PID.ut);
}
