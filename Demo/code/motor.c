#include "motor.h"

PID Motor1_PID;
PID Motor2_PID;
float Motor_target = 0;
//float Motor1_target = 0;   // ���1PIDĿ�꣨������������
//float Motor2_target = 0;   // ���2PIDĿ�꣨������������


void Motor_Init(void)
{
    gpio_init(DIR_R, GPO, GPIO_HIGH, GPO_PUSH_PULL);                           // GPIO ��ʼ��Ϊ��� Ĭ�����������
    pwm_init(PWM_R, 17000, 0);                                                 // PWM ͨ����ʼ��Ƶ�� 17KHz ռ�ձȳ�ʼΪ 0
    gpio_init(DIR_L, GPO, GPIO_HIGH, GPO_PUSH_PULL);                           // GPIO ��ʼ��Ϊ��� Ĭ�����������
    pwm_init(PWM_L, 17000, 0);                                                 // PWM ͨ����ʼ��Ƶ�� 17KHz ռ�ձȳ�ʼΪ 0
    
}

void Motor_L(uint8 dir, uint16 duty)
{
    if(dir == 1)                                                           // ��ת
    {
        if (duty >= DUTY_MAX)
        {
            duty = DUTY_MAX;
        }
      
        gpio_set_level(DIR_L, GPIO_HIGH);                                  // DIR����ߵ�ƽ
        pwm_set_duty(PWM_L, duty * (PWM_DUTY_MAX / DUTY_LIM));                  // ����ռ�ձ�

    }
    else                                                                    // ��ת
    {
      if (duty >= DUTY_MAX)
        {
            duty = DUTY_MAX;
        }
      
        gpio_set_level(DIR_L, GPIO_LOW);                                   // DIR����͵�ƽ
        pwm_set_duty(PWM_L, duty * (PWM_DUTY_MAX / DUTY_LIM));               // ����ռ�ձ�
    }

}

void Motor_R(uint8 dir, uint16 duty)
{
    if(dir == 1)                                                           // ��ת
    {
        if (duty >= DUTY_MAX)
        {
            duty = DUTY_MAX;
        }
      
        gpio_set_level(DIR_R, GPIO_HIGH);                                  // DIR����ߵ�ƽ
        pwm_set_duty(PWM_R, duty * (PWM_DUTY_MAX / DUTY_LIM));                  // ����ռ�ձ�

    }
    else                                                                    // ��ת
    {
        if (duty >= DUTY_MAX)
        {
            duty = DUTY_MAX;
        }
        
        gpio_set_level(DIR_R, GPIO_LOW);                                   // DIR����͵�ƽ
        pwm_set_duty(PWM_R, duty * (PWM_DUTY_MAX / DUTY_LIM));               // ����ռ�ձ�
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

// ���û�������
void Motor1_SetPIDSumLimit(float pLimit)
{
    Motor1_PID.sumLimit = pLimit;
}

// ���������޷�
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

// ���û�������
void Motor2_SetPIDSumLimit(float pLimit)
{
    Motor2_PID.sumLimit = pLimit;
}

// ���������޷�
void Motor2_SetPIDCoLimit(float coLimt)
{
    Motor2_PID.utLimit = coLimt;
}



/**
 * @brief ���PID����
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