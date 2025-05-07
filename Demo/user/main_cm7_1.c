/*********************************************************************************************************************
* CYT4BB Opensourec Library ���� CYT4BB ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
*
* ���ļ��� CYT4BB ��Դ���һ����
*
* CYT4BB ��Դ�� ��������
* �����Ը��������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù������֤��������
* �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����/���޸���
*
* ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı�֤
* ����û�������������Ի��ʺ��ض���;�ı�֤
* ����ϸ����μ� GPL
*
* ��Ӧ�����յ�����Դ���ͬʱ�յ�һ�� GPL �ĸ���
* ���û�У������<https://www.gnu.org/licenses/>
*
* ����ע����
* ����Դ��ʹ�� GPL3.0 ��Դ���֤Э�� �����������Ϊ���İ汾
* �������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ���֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
*
* �ļ�����          main_cm7_1
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          IAR 9.40.1
* ����ƽ̨          CYT4BB
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����              ����                ��ע
* 2024-1-4       pudding            first version
********************************************************************************************************************/

#include "zf_common_headfile.h"
#include "headfile1.h"
// ���µĹ��̻��߹����ƶ���λ�����ִ�����²���
// ��һ�� �ر��������д򿪵��ļ�
// �ڶ��� project->clean  �ȴ��·�����������

// �������ǿ�Դ��չ��� ��������ֲ���߲��Ը���������

// **************************** �������� ****************************



//int m7_1_data;          //����0������

/* ================================
   ���� M7_0 ��������   
   ��� 0��        λ��ƫ��    
   ��� 1��        Ԫ��״̬λ   
   ��� 2��        ��������    �� 1  �� 2  ������ 0  
   ��� 3��        �̶����
   ��� 4��        ����
==================================*/

int search_line_mode = NORMAL_MODE;
//int search_line_mode = LEFT_MODE;

uint8 L_boundary[MT9V03X_H], R_boundary[MT9V03X_H], M_boundary[MT9V03X_H];             // �߽缰��������

static uint8 filted_buffer[188*120];      // �˲��󻺳���   
static uint8 binary_buffer[188*120];      // ��ֵ��������
uint8 img[MT9V03X_H][MT9V03X_W];          // Ԥ�����ͼ��

uint8 threshold = 200;                    // ��ֵ����ֵ��ʼ��

ELEMENT_STATE E;       // Ԫ��״̬�ṹ������
ROUND R;            // ����״̬�ṹ������
 

int main(void)
{
    Main_Init();                  // �ܳ�ʼ��
    
    State_Clear(&E);              // Ԫ��״̬��ʼ��
    Round_Clear(&R);              // ����״̬��ʼ��
    
    while(true)         //һ������
    {
      SCB_CleanInvalidateDCache_by_Addr(&m7_0_data, sizeof(m7_0_data));
        if(m7_0_data[4] == 1)         // ��ȡ KEYx ��ƽΪ��
        {
//          printf("%d", m7_0_data[4]);
            break;
        }
    }
    system_delay_ms(100);
    
    while(true)
    {
        // ���ܵ��µ�֡
      
        
        if (mt9v03x_finish_flag)
        {
            mt9v03x_finish_flag = 0;
            
            // ͼ����
            mean_filter((uint8 *)mt9v03x_image, (uint8 *)filted_buffer);               // ͼ���˲�
            image_binarize((uint8 *)mt9v03x_image, binary_buffer, MT9V03X_W, MT9V03X_H, threshold);             // ��ֵ��
            memcpy(img[0], &binary_buffer[0], MT9V03X_IMAGE_SIZE);               // ͼ�񱸷��ٷ��ͣ��Ա���ͼ��˺��
            
            // ɨ��
            if(search_line_mode == NORMAL_MODE) mid_seek_boundary(img);
            else LR_seek_boundary(img);
            
            // Ԫ��ʶ��
            Roundabout_detect(img, &R, &E);
            StartStopZone_detect(&E);
            Bend_detect(&E);
            OpenRoad_detect(&E, img);
            
//            // ����λ��ƫ��
            m7_0_data[0] = get_offset(&E);    
            SCB_CleanInvalidateDCache_by_Addr(&m7_0_data, sizeof(m7_0_data));
            
            // ��ʾͼ��
            ips114_displayimage03x((const uint8 *)img, MT9V03X_W, MT9V03X_H);    //��ʾͼ��
           
//            get_angel();
//            printf("G:%f\tF:%f\n", Gyro_z, filtered_z_gyro);
        }
       //get_threshold();          // �����ֵ����ֵ
    }
}
