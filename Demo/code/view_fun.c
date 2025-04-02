#include "headfile.h"

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


// ��ȡ�߽������뺽ƫ��
extern uint8 L_boundary[MT9V03X_H], R_boundary[MT9V03X_H], M_boundary[MT9V03X_H];    // �߽缰��������
int seek_boundary(uint8 img[MT9V03X_H][MT9V03X_W])
{
    int left, right, mid = MT9V03X_W/2, index = 0, length = 60, tan, sum_offset = 0;
    // Ѱ��
    left = right = mid;
    while(right < MT9V03X_W) {
        if (img[MT9V03X_H-1][left] == 255) {
            mid = left;
            break;
        }
        if (img[MT9V03X_H-1][right] == 255) {
            mid = right;
            break;
        }
        left --;
        right++;
    }
    // ɨ��
    for (int i = MT9V03X_H-1; i >= MT9V03X_H - length; i--)
    {
        left = right = mid;
        while(left >= 0)
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
        sum_offset += mid - MT9V03X_W/2; 
        M_boundary[index] = mid;
        ips114_draw_point(mid, i, RGB565_BROWN); 
        index ++;
    }
    //tan = (M_boundary[index-1] - MT9V03X_W/2) * 100 / length;
    //return tan;
    return sum_offset;
}



// ************************** Ԫ �� ʶ �� ***************************

// ����
uint8 Roundabout_detect()
{
    
}

// ��ͣ��
uint8 StartStopZone_detect()
{
  
}

// ֱ��
uint8 Bend_detect()
{

}

// ����
uint8 DashedLine_detect()
{

}

// ��·
uint8 OpenRoad_detect()
{

}







