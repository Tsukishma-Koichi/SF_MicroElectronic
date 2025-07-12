#include "headfile1.h"
#include <math.h>

#pragma location = 0x28001000                    // ������������鶨�嵽ָ����RAM��ַ��������������ֱ�ӷ���(��Դ��Ĭ���� 0x28001000 ��ַ������8kb�Ŀռ��������ݽ���)                                                                  
int m7_0_data[5] = {0, 0, 0, 0, 0};   

#define BUZZER_PIN              (P19_4)
//gpio_toggle_level(BUZZER_PIN);����
//gpio_set_level(BUZZER_PIN, GPIO_LOW);�ر�



//   0 ��     255 ��
// *************************** ͼ �� Ԥ �� �� ***************************


// otsu�������ֵ
uint8 otsu_threshold(uint8 *image_data, uint32 width, uint32 height) 
{
    uint32 hist[256] = {0};    // �Ҷ�ֱ��ͼ
    uint32 Gray_min = 255;     // �Ҷ���Сֵ
    uint32 Gray_max = 0;       // �Ҷ����ֵ
    uint64 N = (uint64_t)width * height; // �����������������
    uint64 sum_total = 0;      // �ܻҶ�ֵ
    uint8 threshold = 0;       // ������ֵ

    // ����ֱ��ͼ�������ܻҶ�ֵ����С/���Ҷ�
    for (uint64 i = 0; i < N; i++) 
    {
        uint8 val = image_data[i];
        hist[val]++;
        sum_total += val;
        if (val < Gray_min) Gray_min = val;
        if (val > Gray_max) Gray_max = val;
    }

    // ����ȫͼ��ɫ���
    if (Gray_min == Gray_max) return Gray_min;

    uint64 sum_fore = 0;      // ǰ���ۻ��Ҷ�ֵ
    uint32 count_fore = 0;     // ǰ���ۻ�������
    uint64 max_var = 0;        // �����䷽��

    // ������ʵ�ʴ��ڵĻҶȷ�Χ
    for (uint32 t = Gray_min; t <= Gray_max; t++) 
    {
        count_fore += hist[t];
        sum_fore += (uint64_t)t * hist[t];

        // ������Ч�ָǰ���򱳾�Ϊ�գ�
        uint32 count_back = N - count_fore;
        if (count_fore == 0 || count_back == 0) continue;

        // ���㱳���ܻҶ�ֵ����������
        uint64 sum_back = sum_total - sum_fore;

        // ��������ֵ���������ȣ�
        double mean_fore = (double)sum_fore / count_fore;
        double mean_back = (double)sum_back / count_back;

        // ������䷽��������
        double diff = mean_fore - mean_back;
        uint64 var = (uint64_t)(count_fore * count_back * diff * diff);

        // ������󷽲����ֵ
        if (var > max_var) 
        {
            max_var = var;
            threshold = t;
        }
    }
    return threshold;
}


// ��ֵ��
void image_binarize(uint8 *src, uint8 *dst, uint32 width, uint32 height, uint8 threshold)
{
    uint32 total_pixels = width * height;
    
    // ʹ��ָ��������ٷ���
    uint8 *src_ptr = src;
    uint8 *dst_ptr = dst;
    
    for(uint32 i = 0; i < total_pixels; i++)
    {
        // ��ֵ�жϲ���ֵ��
        *dst_ptr++ = (*src_ptr++ > threshold) ? 0xFF : 0x00;
    }
}


// ͼ���˲�
void mean_filter(uint8 *src, uint8 *dst) {
    // Ԥ�������ۼӣ������ظ����㣩
    uint32 row_sum[MT9V03X_H][MT9V03X_W] = {0};
    for(int y = 0; y < MT9V03X_H; y++) {
        row_sum[y][0] = src[y*MT9V03X_W];
        for(int x = 1; x < MT9V03X_W; x++) {
            row_sum[y][x] = row_sum[y][x-1] + src[y*MT9V03X_W + x];
        }
    }
    
    // ����3x3�����ֵ
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

// Ѱ��
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


// =========== ��ȡ�߽�������λ��ƫ�� =========== //

int bend_offset;

//  �е�̳з�Ѳ��
extern uint8 L_boundary[MT9V03X_H], R_boundary[MT9V03X_H], M_boundary[MT9V03X_H];    // ���ұ߽缰��������
extern int search_line_mode;
uint8 valid_length;

void mid_seek_boundary(uint8 img[MT9V03X_H][MT9V03X_W])
{
    uint8 left, right, mid = MT9V03X_W/2;        // �� �� ��
    uint8 index = 0, length = 120, temp;        // ��������   ɨ�߳��� 
    valid_length= 0;              
    
    // Ѱ��  ���������Ӱ
    for(int i = MT9V03X_H-1; i >= MT9V03X_H-15; i--) {
        mid = seek_white(mid, i, img);
        if (img[i][mid] == 255) {  
            temp = index;
            break;  
        }
        else index ++;
    }
    
    // ɨ��
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


// ���߼̳з�Ѳ��
void LR_seek_boundary(uint8 img[MT9V03X_H][MT9V03X_W])
{
    uint8 left = MT9V03X_W/2, right = MT9V03X_W/2, preleft, preright;
    uint8 index = 0, mid = MT9V03X_W/2, temp = 15;
    valid_length = 0;            // ��Ч����
    
    // Ѱ��  ���������Ӱ
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
    
    // ������ұ����������
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
    
    // ɨ��
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


// ����λ��ƫ��ֵ
int get_offset(ELEMENT_STATE *E)
{
    int sum_offset= 0;      // ���ز���
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






// ************************** Ԫ �� ʶ �� ***************************

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

// -------------------- �� �� --------------------

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
    // ��ֹԪ�س�ͻ
    if(State_Check(E) == 0 || State_Check(E) == ROUNDA)
    {
        // ===================== �뻷�ж� ===================== 
        int jump_L = 6;          // ��������
        int jump_H = 20;         // ��������
        int White_th = 8;        // ������ֵ
        
        if(Round->R_Enter_Flag == 0 && Round->L_Enter_Flag == 0) 
        {   
            int cnt = 0;         // �׵������
            for (int i = 2; i < valid_length; i ++) {
                // �󻷵��½ǵ���
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
                // �һ����½ǵ���
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
        
        // ===================== �����ж� ===================== 
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
        
        // ����
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
        
        // ===================== �����ж� ===================== 
        
        int out_cnt = 0;         // �׵������
        if(Round->R_In_Flag == 1) 
        {   
            for (int i = 2; i < valid_length; i ++) {
                // �󻷵��½ǵ���
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
                // �һ����½ǵ���
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
        // ===================== �����ж� ===================== 
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



// -------------------- �� ͣ �� --------------------
 
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


//  -------------------- ֱ ��  ------------------- 

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


//  --------------------  �� �� -------------------- 

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



//  --------------------  �� · -------------------- 

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




