#include "monitor_re.h"

//int m7_1_data;

// �������ݽ��ջص����� �������һ�����ķ�����Ϣ �˺��Ļᴥ���жϲ��ҿ����ڻص�������ȡ����
//void my_ipc_callback(int receive_data)
//{
//    m7_1_data = receive_data;
////    printf("receive data:%d\r\n", receive_data);        // �����յ������ݴ�ӡ������ 
////    printf("Diff:%d\r\n", Diff_Data);
//}



//------------------------------------------------------------
//״̬��
uint8 Monitor_readFinishStatus = 0;
//------------------------------��������------------------------------
//����
int16 Monitor_DataRead = 0;                        //�ɼ���������
int16 Monitor_Filter[MONITOR_FILTER_MAX + 1];   //�˲����л�����
int16 Monitor_dataPointer = 0;                      //��һ��ָ��,����ѭ������
int16 Monitor_Data = 0;                            //�˲��������
//------------------------------
//�����˲�ʱ���Ȩ��
float Monitor_filterWeight[MONITOR_FILTER_MAX] = {0.8, 0.1, 0.06, 0.04};

/**
 * @brief   ��ȡһ�����ݲ������˲�
 * @return  NULL
 */
void Monitor_ReRead(void) {
    Monitor_readFinishStatus = 0;
    //���ݲɼ��Ĺ���
    
    Monitor_DataRead = m7_0_data[0];
    Monitor_Data = 0;
    //����һ���������ڵ��˲�
    Monitor_Filter[Monitor_dataPointer] = Monitor_DataRead;
    for (int i = 0; i < MONITOR_FILTER_MAX; ++i) {
        int index = Monitor_dataPointer - i;
        if (index < 0)
            index += MONITOR_FILTER_MAX;
        Monitor_Data += Monitor_Filter[index] * Monitor_filterWeight[i];
    }
    Monitor_dataPointer = (++Monitor_dataPointer) % MONITOR_FILTER_MAX;
    Monitor_readFinishStatus = 1;
}