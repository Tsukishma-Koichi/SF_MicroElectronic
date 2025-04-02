#include "pid.h"


/**
 * @brief           PID初始化
 * @param p         待初始化的指针
 * @return          NULL
 */
void PID_Init(PID *p)
{
    p->kP = 0.0;
    p->kI = 0.0;
    p->kD = 0.0;
    p->kPSet = 0.0;
    p->kISet = 0.0;
    p->kDSet = 0.0;
    p->sum = 0.0;
    p->preError = 0.0;
    p->ppreError = 0.0;
    p->ut = 0.0;
    p->sumLimit = 0.0;
    p->utLimit = 0.0;
}

/**
 * @brief               整体设置p的各项参数
 * @param pLimit        积分限幅
 * @param coLimit       修正限幅
 * @return              NULL
 */
void PID_SetParameter(PID *p, float setP, float setI, float setD, float pLimit, float coLimit, float boost)
{
    p->kPSet = boost * setP;
    p->kISet = boost * setI;
    p->kDSet = boost * setD;
    p->sumLimit = pLimit;
    p->utLimit = coLimit;
    p->kP = p->kPSet;
    p->kI = p->kISet;
    p->kD = p->kDSet;
}



/**
 * @brief               位置式PID
 * @param   p           用来工作的PID参数
 * @param   target      目标值
 * @param   cur         当前值
 */
void PID_PostionalPID(PID *p, float target, float cur)
{
    float error = target - cur;
    // 误差积分
    p->sum += error * p->kI;
    // 进行积分限幅
    p->sum = p->sum > p->sumLimit ? p->sumLimit : p->sum;
    p->sum = p->sum < -p->sumLimit ? -p->sumLimit : p->sum;
    // 数据进行计算
    if (-1 < error - p->preError && error - p->preError < 1)
    {
        p->ut = p->kP * error;
    }
    else
    {
        p->ut = p->kP * error + p->sum + p->kD * (error - p->preError);
    }
    // 进行修正限幅
    p->ut = p->ut > p->utLimit ? p->utLimit : p->ut;
    p->ut = p->ut < -p->utLimit ? -p->utLimit : p->ut;
    // 进行数据更新
    p->preError = error;
}

/**
 * @brief               增量式PID
 * @param p             用来工作的PID参数
 * @param target        目标值
 * @param cur           当前值
 *
 */
void PID_IncrementalPID(PID *p, float target, float cur)
{
    float error = target - cur;
    // 数据进行增加
    p->ut += p->kP * (error - p->preError) + p->kI * error + p->kD * (error - 2 * p->preError + p->preError);
    // 进行修正限幅
    p->ut = p->ut > p->utLimit ? p->utLimit : p->ut;
    p->ut = p->ut < -p->utLimit ? -p->utLimit : p->ut;
    // 进行数据更新
    p->ppreError = p->preError;
    p->preError = error;
}

/**
 * @brief           2PD初始化
 * @param p         待初始化的指针
 * @return          NULL
 */
void PD_Init(PD *p)
{
    p->kP1 = 0.0;
    p->kD1 = 0.0;
    p->kP2 = 0.0;
    p->kD2 = 0.0;
    p->preError = 0.0;
    p->ppreError = 0.0;
    p->ut = 0.0;
    p->utLimit = 0.0;
}

/**
 * @brief               PPD
 * @param   p           用来工作的PID参数
 * @param   target      目标值
 * @param   cur         当前值
 */
void PID_2PD(PD *p, float target, float cur)
{
    float error = target - cur;
    // 数据进行计算
    
    p->ut = p->kP1 * error + p->kP2 * fabs(error) * error + p->kD1 * (error - p->preError) + p->kD2 * filtered_z_gyro;
    
    // 进行修正限幅
    p->ut = p->ut > p->utLimit ? p->utLimit : p->ut;
    p->ut = p->ut < -p->utLimit ? -p->utLimit : p->ut;
    // 进行数据更新
    p->preError = error;
}

/**
 * @brief               整体设置p的各项参数
 * @param coLimit       修正限幅
 * @return              NULL
 */
void PD_SetParameter(PD *p, float P1, float P2, float D1, float D2, float coLimit, float boost)
{
    p->utLimit = coLimit;
    p->kP1 = P1;
    p->kP2 = P2;
    p->kD1 = D1;
    p->kD2 = D2;
}