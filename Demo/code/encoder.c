#include "encoder.h"

int16 encoder_data_quad[2] = {0};



void Encoder_Init(void)
{
	encoder_quad_init(ENCODER_QUAD1, ENCODER_QUAD1_PHASE_A, ENCODER_QUAD1_PHASE_B);  // 初始化编码器模块与引脚 正交编码器模式
	encoder_quad_init(ENCODER_QUAD2, ENCODER_QUAD2_PHASE_A, ENCODER_QUAD2_PHASE_B);  // 初始化编码器模块与引脚 正交编码器模式
	
	                                                     // 初始化 PIT0 为周期中断 100ms 周
}


void Encoder_print(void)
{
	    encoder_data_quad[0] = -encoder_get_count(ENCODER_QUAD1);
	    encoder_clear_count(ENCODER_QUAD1);
	    encoder_data_quad[1] = encoder_get_count(ENCODER_QUAD2);
	    encoder_clear_count(ENCODER_QUAD2);
	    
	    printf("LCounter \t\t%04d\r\n", encoder_data_quad[0]);      // 输出编码器计数信息
        
        printf("RCounter \t\t%04d\r\n", encoder_data_quad[1]);      // 输出编码器计数信息

}


uint16 Encoder_GetCnt(uint8 RorL)
{
    switch (RorL)
    {
    case 'L':
      {
        encoder_data_quad[0] = -encoder_get_count(ENCODER_QUAD1);
        encoder_clear_count(ENCODER_QUAD1);
        return encoder_data_quad[0];
      }
    case 'R':
      {
        encoder_data_quad[1] = encoder_get_count(ENCODER_QUAD2);
        encoder_clear_count(ENCODER_QUAD2);
        return encoder_data_quad[1];
      }
    }
    return 0;
}
       


