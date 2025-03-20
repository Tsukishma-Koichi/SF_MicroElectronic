#ifndef _ENCODER_H_
#define _ENCODER_H_

#include "zf_common_headfile.h"



#define ENCODER_QUAD1                    (TC_CH58_ENCODER)                      // 编码器接口1    
#define ENCODER_QUAD1_PHASE_A            (TC_CH58_ENCODER_CH1_P17_3)            // PHASE_A 对应的引脚                     
#define ENCODER_QUAD1_PHASE_B            (TC_CH58_ENCODER_CH2_P17_4)            // PHASE_B 对应的引脚                        
                                                                                
#define ENCODER_QUAD2                    (TC_CH27_ENCODER)                      // 编码器接口2 
#define ENCODER_QUAD2_PHASE_A            (TC_CH27_ENCODER_CH1_P19_2)            // PHASE_A 对应的引脚                  
#define ENCODER_QUAD2_PHASE_B            (TC_CH27_ENCODER_CH2_P19_3)            // PHASE_B 对应的引脚      

#define R   'R'
#define L   'L'
                                                                                
extern int16 encoder_data_quad[];

void Encoder_Init(void);
void Encoder_print(void);
uint16 Encoder_GetCnt(uint8 RorL);


#endif