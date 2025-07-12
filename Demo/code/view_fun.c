#include "headfile1.h"
#include <math.h>

#pragma location = 0x28001000                    // 将下面这个数组定义到指定的RAM地址，便于其他核心直接访问(开源库默认在 0x28001000 地址保留了8kb的空间用于数据交互)                                                                  
int m7_0_data[5] = {0, 0, 0, 0, 0};   

#define BUZZER_PIN              (P19_4)
//gpio_toggle_level(BUZZER_PIN);开启
//gpio_set_level(BUZZER_PIN, GPIO_LOW);关闭



//   0 黑     255 白
// *************************** 图 像 预 处 理 ***************************


// otsu大津法算阈值
uint8 otsu_threshold(uint8 *image_data, uint32 width, uint32 height) 
{
    uint32 hist[256] = {0};    // 灰度直方图
    uint32 Gray_min = 255;     // 灰度最小值
    uint32 Gray_max = 0;       // 灰度最大值
    uint64 N = (uint64_t)width * height; // 总像素数（防溢出）
    uint64 sum_total = 0;      // 总灰度值
    uint8 threshold = 0;       // 最终阈值

    // 生成直方图并计算总灰度值、最小/最大灰度
    for (uint64 i = 0; i < N; i++) 
    {
        uint8 val = image_data[i];
        hist[val]++;
        sum_total += val;
        if (val < Gray_min) Gray_min = val;
        if (val > Gray_max) Gray_max = val;
    }

    // 处理全图单色情况
    if (Gray_min == Gray_max) return Gray_min;

    uint64 sum_fore = 0;      // 前景累积灰度值
    uint32 count_fore = 0;     // 前景累积像素数
    uint64 max_var = 0;        // 最大类间方差

    // 仅遍历实际存在的灰度范围
    for (uint32 t = Gray_min; t <= Gray_max; t++) 
    {
        count_fore += hist[t];
        sum_fore += (uint64_t)t * hist[t];

        // 跳过无效分割（前景或背景为空）
        uint32 count_back = N - count_fore;
        if (count_fore == 0 || count_back == 0) continue;

        // 计算背景总灰度值（修正错误）
        uint64 sum_back = sum_total - sum_fore;

        // 浮点计算均值（保留精度）
        double mean_fore = (double)sum_fore / count_fore;
        double mean_back = (double)sum_back / count_back;

        // 计算类间方差（防溢出）
        double diff = mean_fore - mean_back;
        uint64 var = (uint64_t)(count_fore * count_back * diff * diff);

        // 更新最大方差和阈值
        if (var > max_var) 
        {
            max_var = var;
            threshold = t;
        }
    }
    return threshold;
}


// 二值化
void image_binarize(uint8 *src, uint8 *dst, uint32 width, uint32 height, uint8 threshold)
{
    uint32 total_pixels = width * height;
    
    // 使用指针运算加速访问
    uint8 *src_ptr = src;
    uint8 *dst_ptr = dst;
    
    for(uint32 i = 0; i < total_pixels; i++)
    {
        // 阈值判断并二值化
        *dst_ptr++ = (*src_ptr++ > threshold) ? 0xFF : 0x00;
    }
}


// 图像滤波
void mean_filter(uint8 *src, uint8 *dst) {
    // 预计算行累加（减少重复计算）
    uint32 row_sum[MT9V03X_H][MT9V03X_W] = {0};
    for(int y = 0; y < MT9V03X_H; y++) {
        row_sum[y][0] = src[y*MT9V03X_W];
        for(int x = 1; x < MT9V03X_W; x++) {
            row_sum[y][x] = row_sum[y][x-1] + src[y*MT9V03X_W + x];
        }
    }
    
    // 计算3x3区域均值
    for(int y = 1; y < MT9V03X_H-1; y++) {
        for(int x = 1; x < MT9V03X_W-1; x++) {
            uint32 sum = 0;
            for(int j = -1; j <= 1; j++) {
                sum += row_sum[y+j][x+1] - row_sum[y+j][x-2];
            }
            dst[y*MT9V03X_W + x] = sum / 9;
        }
    }
}

uint8 miss_line = 0;

// 寻白
uint8 seek_white(uint8 mid, uint8 row, uint8 img[MT9V03X_H][MT9V03X_W])
{
    miss_line = 0;
    uint8 left = mid;
    uint8 right = mid;
    while(right < MT9V03X_W && left > 0) {
        if (img[row][left] == 255) {
            mid = left;   
            break;
        }
        if (img[row][right] == 255) {
            mid = right;
            break;
        }
        left --;
        right++;
        if(left == 0) {
            miss_line = MT9V03X_H - row;
        } 
    }
    
    return mid;
}


// =========== 获取边界数组与位置偏差 =========== //

int bend_offset;

//  中点继承法巡线
extern uint8 L_boundary[MT9V03X_H], R_boundary[MT9V03X_H], M_boundary[MT9V03X_H];    // 左右边界及中线数组
extern int search_line_mode;
uint8 valid_length;

void mid_seek_boundary(uint8 img[MT9V03X_H][MT9V03X_W])
{
    uint8 left, right, mid = MT9V03X_W/2;        // 左 右 中
    uint8 index = 0, length = 120, temp;        // 数组序列   扫线长度 
    valid_length= 0;              
    
    // 寻白  处理近处阴影
    for(int i = MT9V03X_H-1; i >= MT9V03X_H-15; i--) {
        mid = seek_white(mid, i, img);
        if (img[i][mid] == 255) {  
            temp = index;
            break;  
        }
        else index ++;
    }
    
    // 扫线
    for (int i = MT9V03X_H-1-temp; i > MT9V03X_H - length; i --)
    {
        left = mid;
        right = mid;
        while(left > 0)
        {
            if (img[i][left] == 0) {
                L_boundary[index] = left;
                ips114_draw_point(left, i, RGB565_BLUE); 
                break;
            }
            else left --;
        }
        while(right < MT9V03X_W)
        {
            if (img[i][right] == 0) {
                R_boundary[index] = right;
                ips114_draw_point(right, i, RGB565_GREEN); 
                break;
            }
            else right ++;
        }
        mid = (right + left) / 2;
        M_boundary[index] = mid;
        if(temp == index) {
            for(int j = 0; j < temp; j++) {
                M_boundary[j] = mid;
                L_boundary[j] = left;
                R_boundary[j] = right;
                ips114_draw_point(mid, MT9V03X_H - 1 - j, RGB565_BROWN); 
                ips114_draw_point(left, MT9V03X_H - 1 - j, RGB565_BLUE); 
                ips114_draw_point(right, MT9V03X_H - 1 - j, RGB565_GREEN); 
            }
        }
        if(mid == left && mid == right && valid_length == 0) {
            valid_length = index;
        }
        ips114_draw_point(mid, i, RGB565_BROWN); 
        index ++;
    }
    if (valid_length == 0) valid_length = 119;
}


// 边线继承法巡线
void LR_seek_boundary(uint8 img[MT9V03X_H][MT9V03X_W])
{
    uint8 left = MT9V03X_W/2, right = MT9V03X_W/2, preleft, preright;
    uint8 index = 0, mid = MT9V03X_W/2, temp = 15;
    valid_length = 0;            // 有效长度
    
    // 寻白  处理近处阴影
    for(int i = MT9V03X_H-1; i >= MT9V03X_H-temp; i--) {
        mid = seek_white(mid, i, img);   
        if (img[i][mid] == 255) {
            left = mid;
            right = mid;
            temp = index;  
            break;  
        }  
        else index ++;  
    }
    
    // 获得左右边线生长起点
    while(left > 0) {
        if (img[MT9V03X_H-1-temp][left] == 0) {
             L_boundary[temp] = left;
             ips114_draw_point(left, MT9V03X_H-1-temp, RGB565_BLUE); 
             break;
         }
         else left --;
    }
    while(right < MT9V03X_W) {
        if (img[MT9V03X_H-1-temp][right] == 0) {
            R_boundary[temp] = right;
            ips114_draw_point(right, MT9V03X_H-1-temp, RGB565_RED); 
            break;
        }
        else right ++;
    }
    mid = (left + right) / 2;
    preleft = left;
    preright = right;
    for(int j = 0; j < temp; j++) {
        M_boundary[j] = mid;
        L_boundary[j] = left;
        R_boundary[j] = right;
        ips114_draw_point(mid, MT9V03X_H - 1 - j, RGB565_BROWN); 
        ips114_draw_point(left, MT9V03X_H - 1 - j, RGB565_BLUE); 
        ips114_draw_point(right, MT9V03X_H - 1 - j, RGB565_RED); 
    }
    
    // 扫线
    for (int i = MT9V03X_H-2-temp; i >= 50; i --) {
        index ++;
        if (valid_length == 0) {
            while(left > 0 && left < MT9V03X_W) {
              if(img[i][preleft] == 0) {
                  left ++;
                  if (img[i][left] == 255) {
                      preleft = left-1;
                      break;
                  }
              }
              else {
                  left --;
                  if (img[i][left] == 0) {
                      preleft = left;
                      break;
                  }
              }
            }
            while(right > 0 && right < MT9V03X_W) {
                if(img[i][preright] == 0) {
                      right --;
                      if (img[i][right] == 255) {
                          preright = right+1;
                          break;
                      }
                }
                else {
                   right ++;
                   if (img[i][right] == 0) {
                       preright = right;
                       break;
                   }
                }
            }
//            printf("L%dR%d", left, right);
//            if(left >= right) valid_length = index; 
        }
        L_boundary[index] = preleft;
        R_boundary[index] = preright;
        M_boundary[index] = (preleft + preright) / 2;
        ips114_draw_point(preright, i, RGB565_RED); 
        ips114_draw_point(preleft, i, RGB565_BLUE); 
        ips114_draw_point(M_boundary[index], i, RGB565_BROWN); 
    }
    if (valid_length == 0) valid_length = index;
}


// 计算位置偏差值
int get_offset(ELEMENT_STATE *E)
{
    int sum_offset= 0;      // 返回参数
    for (int i = 0;i<= 80;i ++){
        if(i <= 50 && i >= 20) {// || (i >= 45 && i <= 60)) {
            switch(search_line_mode) 
            {
                case NORMAL_MODE:
                  {sum_offset += M_boundary[i] -  MT9V03X_W/2;} break;
                case 1:
                  {sum_offset += L_boundary[i] -  MT9V03X_W/2;} break;
                case RIGHT_MODE:
                  {sum_offset += R_boundary[i] -  MT9V03X_W/2;} break;
            }      
        }
        
    }

//    printf("FLAG:%d\nVL:%d", search_line_mode, valid_length);
    if(E->Bend_Flag == 1) return bend_offset*0.8;
    return sum_offset/5;
}






// ************************** 元 素 识 别 ***************************

void State_Clear(ELEMENT_STATE *E)
{
    if(State_Check(E)) {
        ips114_clear();
        E->Round_Flag = 0;
        E->Bend_Flag = 0;
        E->SSZone_Flag = 0;
        E->OR_Flag = 0;
        m7_0_data[1] = 0;
        m7_0_data[2] = 0;
    }
}


uint8 State_Check(ELEMENT_STATE *E)
{
  if(E->Round_Flag == 1) {
      ips114_show_string(190, 10, "Round");
      return ROUNDA;
  }
  else if(E->Bend_Flag >= 1) {
      ips114_show_string(190, 10, "Bend");
      return BEND;
  }
  else if(E->SSZone_Flag >= 1) {
      ips114_show_string(190, 10, "SSZone");
      return SSZ;
  }
  else if(E->OR_Flag == 1) {
      ips114_show_string(190, 10, "OR");
      return OR;
  }
  else return 0;
}

// -------------------- 环 岛 --------------------

void Round_Clear(ROUND *Round)
{
    Round->R_Enter_Flag = 0;
    Round->L_Enter_Flag = 0;
    Round->R_In_Flag = 0;
    Round->L_In_Flag = 0;
    Round->R_Out_Flag = 0;
    Round->L_Out_Flag = 0;
    Round->Finish_Flag= 0;
}


void Roundabout_detect(uint8 img[MT9V03X_H][MT9V03X_W], ROUND *Round, ELEMENT_STATE*E)
{
    // 防止元素冲突
    if(State_Check(E) == 0 || State_Check(E) == ROUNDA)
    {
        // ===================== 入环判断 ===================== 
        int jump_L = 6;          // 跳变下限
        int jump_H = 20;         // 跳变上限
        int White_th = 8;        // 白线阈值
        
        if(Round->R_Enter_Flag == 0 && Round->L_Enter_Flag == 0) 
        {   
            int cnt = 0;         // 白点计数器
            for (int i = 2; i < valid_length; i ++) {
                // 左环岛下角点检测
                if(L_boundary[i-1] - L_boundary[i] > jump_L && L_boundary[i] - L_boundary[i+1] < jump_H) {
                    for(int j = 1; j <= i; j ++) {
                        if (img[MT9V03X_H-1-i+j][L_boundary[i]] == 255) cnt ++;
                        else{ 
                            cnt = 0;
                            break;
                        }
                        if (cnt >= White_th) {
                            Round->L_Enter_Flag = 1; 
                            E->Round_Flag = 1;
                            search_line_mode= LEFT_MODE;
                            ips114_show_string(155, 115, "EnterLR");
                            gpio_toggle_level(BUZZER_PIN);
                            break;
                        }
                    }
                    if (Round->R_Enter_Flag== 1) break;
                } 
                // 右环岛下角点检测
                if(R_boundary[i] - R_boundary[i-1] > jump_L && R_boundary[i] - R_boundary[i-1] < jump_H) {
                    for(int j = 1; j <= i; j ++) {
                        if (img[MT9V03X_H-1-i+j][R_boundary[i]] == 255) cnt ++;
                        else{ 
                            cnt = 0;
                            break;
                        }
                        if (cnt >= White_th) {
                            Round->R_Enter_Flag = 1; 
                            E->Round_Flag = 1;
                            search_line_mode= RIGHT_MODE;
                            ips114_show_string(155, 115, "EnterRR");
                            gpio_toggle_level(BUZZER_PIN);
                            break;
                        }
                    }
                    if (Round->L_Enter_Flag== 1) break;
                } 
            }
        }
        
        // ===================== 环中判断 ===================== 
        if(Round->R_Enter_Flag == 1 && Round->R_In_Flag == 0) 
        {
            get_angel();
//            printf("RI %f\r\n", Gyro_z);
            if(Gyro_z <= -180) {
                  search_line_mode= LEFT_MODE;
                  Round->R_In_Flag = 1;
                  ips114_clear();
                  ips114_show_string(155, 115, "InRR");
                  gpio_set_level(BUZZER_PIN, GPIO_LOW);
            }
        }
        
        // 左环中
        if(Round->L_Enter_Flag == 1 && Round->L_In_Flag == 0)
        {
            get_angel();
//            printf("LI %f\r\n", Gyro_z);
            if(Gyro_z >= 180) {
                  search_line_mode= RIGHT_MODE;
                  Round->L_In_Flag = 1;
                  ips114_clear();
                  ips114_show_string(155, 115, "InLR");
                  gpio_set_level(BUZZER_PIN, GPIO_LOW);
            }
        }
        
        // ===================== 出环判断 ===================== 
        
        int out_cnt = 0;         // 白点计数器
        if(Round->R_In_Flag == 1) 
        {   
            for (int i = 2; i < valid_length; i ++) {
                // 左环岛下角点检测
                if(L_boundary[i-1] - L_boundary[i] > jump_L && L_boundary[i] - L_boundary[i+1] < jump_H) {
                    for(int j = 1; j <= i; j ++) {
                        if (img[MT9V03X_H-1-i+j][L_boundary[i]] == 255) out_cnt ++;
                        else{ 
                            out_cnt = 0;
                            break;
                        }
                        if (out_cnt >= White_th) {
                            Round->L_Out_Flag = 1; 
                            ips114_show_string(155, 115, "outRR");
                            gpio_toggle_level(BUZZER_PIN);
                            break;
                        }
                    }
                    if (Round->R_Out_Flag== 1) break;
                } 
                
            }
        }
        if(Round->L_In_Flag == 1) 
        {
            for (int i = 2; i < valid_length; i ++) {
                // 右环岛下角点检测
                if(R_boundary[i] - R_boundary[i-1] > jump_L && R_boundary[i] - R_boundary[i-1] < jump_H) {
                    for(int j = 1; j <= i; j ++) {
                        if (img[MT9V03X_H-1-i+j][R_boundary[i]] == 255) out_cnt ++;
                        else{ 
                            out_cnt = 0;
                            break;
                        }
                        if (out_cnt >= White_th) {
                            Round->R_Out_Flag = 1; 
                            ips114_show_string(155, 115, "OutLR");
                            gpio_toggle_level(BUZZER_PIN);
                            break;
                        }
                    }
                    if (Round->L_Out_Flag== 1) break;
                } 
            }
        }
        // ===================== 结束判断 ===================== 
        if(Round->R_Out_Flag == 1 || Round->L_Out_Flag == 1) 
        {
            for(int i = 5; i <= 50; i++)
            {
                if(abs(L_boundary[i+1] -  L_boundary[0]) > 5 || abs(R_boundary[i+1] -  R_boundary[0]) > 5)
                    break;
                if(i == 50) {
                    Round->Finish_Flag = 1;
                }
            }
            if(Round->Finish_Flag == 1) {
                search_line_mode= NORMAL_MODE;
                Round_Clear(Round);
                State_Clear(E);
                ips114_clear();
                gpio_set_level(BUZZER_PIN, GPIO_LOW);
            }
            
        }
    }
}



// -------------------- 启 停 区 --------------------
 
void StartStopZone_detect(ELEMENT_STATE*E)
{
    if(State_Check(E) == 0 || State_Check(E) == SSZ) {
        for (int i = 0; i <= 5; i++)
        {
          if(R_boundary[i] - L_boundary[i] >= 60 && abs(M_boundary[i] - 94) < 12) {
              E->SSZone_Flag = 1;
              m7_0_data[1] = SSZ;
              ips114_show_string(155, 115, "StopZone");
              break;
          }
          if (i == 5) {
              State_Clear(E);
              m7_0_data[1] = 0;
          }
        }
    }
}


//  -------------------- 直 角  ------------------- 

void Bend_detect(ELEMENT_STATE*E)
{
    if(State_Check(E) == 0 || State_Check(E) == BEND) {
        int L_flag = 0, R_flag = 0;  
        int jump_th = 25;
        if(valid_length < 50 && valid_length > 20) {
            for (int i = 3; i < valid_length; i++) 
            {
                if(L_boundary[i-2] - L_boundary[i] >= jump_th) L_flag = i;
                if(R_boundary[i] - R_boundary[i-2] >= jump_th) R_flag = i;
            }
        }
        if((L_flag == 0 && R_flag == 0) || (L_flag > 0 && R_flag > 0)) {
            State_Clear(E);
            gpio_set_level(BUZZER_PIN, GPIO_LOW);
        }
        else if(L_flag > 0) {
            if(abs(R_boundary[L_flag-1] - R_boundary[0]) <= 8) {
                ips114_show_string(155, 115, "LeftBend");
                gpio_toggle_level(BUZZER_PIN);
                E->Bend_Flag = 1;
                m7_0_data[1] = BEND;
                m7_0_data[2] = LEFT;
                bend_offset = L_flag * (M_boundary[L_flag] - MT9V03X_W / 2) / 2;
            }
        }
        else{
            if(abs(L_boundary[R_flag-1] - L_boundary[0]) <= 8) {
                ips114_show_string(155, 115, "RightBend");
                gpio_toggle_level(BUZZER_PIN);
                E->Bend_Flag = 1;
                m7_0_data[1] = BEND;
                m7_0_data[2] = RIGHT;
                bend_offset = R_flag * (M_boundary[R_flag] - MT9V03X_W / 2) / 2;
            }
        }
    }
}


//  --------------------  虚 线 -------------------- 

void DashedLine_detect(ELEMENT_STATE*E, uint8 img[MT9V03X_H][MT9V03X_W])
{
    if(State_Check(E) == 0 || State_Check(E) == OR) {
      for (int i = 0; i<= 45; i++) {
        if(img[MT9V03X_H-1-i][M_boundary[i]] != 0) {
              break;
        }
        if(i == 45) {
            ips114_show_string(155, 115, "OpenRoad");
            m7_0_data[1] = OR;
            E->OR_Flag = 1;
        }
      }
      if (E->OR_Flag == 1 && valid_length >= 60) {
          m7_0_data[1] = 0;
          State_Clear(E);
          ips114_clear();
      }
    }
}



//  --------------------  断 路 -------------------- 

void OpenRoad_detect(ELEMENT_STATE*E, uint8 img[MT9V03X_H][MT9V03X_W])
{
    if(State_Check(E) == 0 || State_Check(E) == OR) {
      for (int i = 0; i<= 45; i++) {
        if(img[MT9V03X_H-1-i][M_boundary[i]] != 0) {
              break;
        }
        if(i == 45) {
            ips114_show_string(155, 115, "OpenRoad");
            m7_0_data[1] = OR;
            E->OR_Flag = 1;
        }
      }
      if (E->OR_Flag == 1 && valid_length >= 60) {
          m7_0_data[1] = 0;
          State_Clear(E);
          ips114_clear();
      }
    }
}




