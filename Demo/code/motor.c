#include "motor.h"

PID Motor1_PID;
PID Motor2_PID;
float Motor_target = 0;
//float Motor1_target = 0;   // 电机1PID目标（编码器读数）
//float Motor2_target = 0;   // 电机2PID目标（编码器读数）


void Motor_Init(void)
{
    gpio_init(DIR_R, GPO, GPIO_HIGH, GPO_PUSH_PULL);                           // GPIO 初始化为输出 默认上拉输出高
    pwm_init(PWM_R, 17000, 0);                                                 // PWM 通道初始化频率 17KHz 占空比初始为 0
    gpio_init(DIR_L, GPO, GPIO_HIGH, GPO_PUSH_PULL);                           // GPIO 初始化为输出 默认上拉输出高
    pwm_init(PWM_L, 17000, 0);                                                 // PWM 通道初始化频率 17KHz 占空比初始为 0
    
}

void Motor_L(uint8 dir, uint16 duty)
{
    if(dir == 1)                                                           // 正转
    {
        if (duty >= DUTY_MAX)
        {
            duty = DUTY_MAX;
        }
      
        gpio_set_level(DIR_L, GPIO_HIGH);                                  // DIR输出高电平
        pwm_set_duty(PWM_L, duty * (PWM_DUTY_MAX / DUTY_LIM));                  // 计算占空比

    }
    else                                                                    // 反转
    {
      if (duty >= DUTY_MAX)
        {
            duty = DUTY_MAX;
        }
      
        gpio_set_level(DIR_L, GPIO_LOW);                                   // DIR输出低电平
        pwm_set_duty(PWM_L, duty * (PWM_DUTY_MAX / DUTY_LIM));               // 计算占空比
    }

}

void Motor_R(uint8 dir, uint16 duty)
{
    if(dir == 1)                                                           // 正转
    {
        if (duty >= DUTY_MAX)
        {
            duty = DUTY_MAX;
        }
      
        gpio_set_level(DIR_R, GPIO_HIGH);                                  // DIR输出高电平
        pwm_set_duty(PWM_R, duty * (PWM_DUTY_MAX / DUTY_LIM));                  // 计算占空比

    }
    else                                                                    // 反转
    {
        if (duty >= DUTY_MAX)
        {
            duty = DUTY_MAX;
        }
        
        gpio_set_level(DIR_R, GPIO_LOW);                                   // DIR输出低电平
        pwm_set_duty(PWM_R, duty * (PWM_DUTY_MAX / DUTY_LIM));               // 计算占空比
    }

}

PID Motor1_PID;
PID Motor2_PID;


void Motor_PID_Init(void)
{
    PID_Init(&Motor1_PID);
    PID_Init(&Motor2_PID);
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



/**
 * @brief 电机PID运行
 * 
 */
void MotorR_PIDwork(float MotorR_target)
{
    PID_PostionalPID(&Motor1_PID, MotorR_target, Encoder_GetCnt(R));
    Motor_R(1, (int16)Motor1_PID.ut);
}

void MotorL_PIDwork(float MotorL_target)
{
    PID_PostionalPID(&Motor2_PID, MotorL_target, Encoder_GetCnt(L));
    Motor_L(1, (int16)Motor2_PID.ut);
}