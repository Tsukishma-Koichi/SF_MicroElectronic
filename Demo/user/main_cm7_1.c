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
#include "common.h"
#include "view_fun.h"
// ���µĹ��̻��߹����ƶ���λ�����ִ�����²���
// ��һ�� �ر��������д򿪵��ļ�
// �ڶ��� project->clean  �ȴ��·�����������

// �������ǿ�Դ��չ��� ��������ֲ���߲��Ը���������
// �������ǿ�Դ��չ��� ��������ֲ���߲��Ը���������
// �������ǿ�Դ��չ��� ��������ֲ���߲��Ը���������

// **************************** �������� ****************************


uint8 L_boundary[MT9V03X_H], R_boundary[MT9V03X_H], M_boundary[MT9V03X_H];             // �߽缰��������

static uint8 filted_buffer[188*120];      // �˲��󻺳���
static uint8 binary_buffer[188*120];      // ��ֵ��������
uint8 img[MT9V03X_H][MT9V03X_W];          // Ԥ�����ͼ��

uint8 threshold = 128;                    // ��ֵ����ֵ
int send_data = 0;                        // Ԥ���ݲ���
uint8 pit_00_state;                       // 00�жϱ�־λ

int flag = 10;

int main(void)
{
    Main_Init();       
    
    while(true)
    {
        // ���ܵ��µ�֡
        if (mt9v03x_finish_flag)
        {
            mt9v03x_finish_flag = 0;
            
            // ͼ����
            //mean_filter((uint8 *)mt9v03x_image, (uint8 *)filted_buffer);               // ͼ���˲�
            
            image_binarize((uint8 *)mt9v03x_image, binary_buffer, MT9V03X_W, MT9V03X_H, threshold);             // ��ֵ��
            
            memcpy(img[0], &binary_buffer[0], MT9V03X_IMAGE_SIZE);               // �ڷ���ǰ��ͼ�񱸷��ٽ��з��ͣ��������Ա���ͼ�����˺�ѵ�����
            
            send_data = seek_boundary(img);                  // ɨ��
            
            ipc_send_data(send_data);                        // �������ݸ�����M7_0
       
            ips114_displayimage03x((const uint8 *)img, MT9V03X_W, MT9V03X_H);    //��ʾͼ��
        }
        
        // ÿ��5�������ֵ����ֵ
        if (pit_00_state)
        {
            //threshold = otsu_threshold((uint8 *)mt9v03x_image, MT9V03X_W, MT9V03X_H);
            threshold = 200;
            //printf("threshold = %d\r\n", threshold);
            pit_00_state = 0;                                                      // ��������жϴ�����־λ
        }
    }
}






//int main(void)
//{
//    clock_init(SYSTEM_CLOCK_250M); 	// ʱ�����ü�ϵͳ��ʼ��<��ر���>
//    debug_info_init();                  // ���Դ�����Ϣ��ʼ��
//     
//    ips114_init();
//    ips114_show_string(0, 0, "mt9v03x init.");
//    while(1)
//    {
//        if(mt9v03x_init())
//            ips114_show_string(0, 16, "mt9v03x reinit.");
//        else
//            break;
//        system_delay_ms(500);                                                   // ����ʱ�������Ʊ�ʾ�쳣
//    }
//    ips114_show_string(0, 16, "init success.");
//    
//    
//    // �˴���д�û����� ���������ʼ�������
//    while(true)
//    {
//        // �˴���д��Ҫѭ��ִ�еĴ���
//
//        if(mt9v03x_finish_flag)
//        {
//            ips114_displayimage03x((const uint8 *)mt9v03x_image, MT9V03X_W, MT9V03X_H);                             // ��ʾԭʼͼ��
////            ips114_show_gray_image(0, 0, (const uint8 *)mt9v03x_image, MT9V03X_W, MT9V03X_H, 240, 135, 64);       // ��ʾ�Ҷ�ͼ��
//            mt9v03x_finish_flag = 0;
//        }
//      
//      
//        // �˴���д��Ҫѭ��ִ�еĴ���
//    }
//}

// **************************** �������� ****************************
