#include "headfile.h"

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


// 获取边界数组与航偏角
extern uint8 L_boundary[MT9V03X_H], R_boundary[MT9V03X_H], M_boundary[MT9V03X_H];    // 边界及中线数组
int seek_boundary(uint8 img[MT9V03X_H][MT9V03X_W])
{
    int left, right, mid = MT9V03X_W/2, index = 0, length = 60, tan, sum_offset = 0;
    // 寻白
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
    // 扫线
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



// ************************** 元 素 识 别 ***************************

// 环岛
uint8 Roundabout_detect()
{
    
}

// 启停区
uint8 StartStopZone_detect()
{
  
}

// 直角
uint8 Bend_detect()
{

}

// 虚线
uint8 DashedLine_detect()
{

}

// 断路
uint8 OpenRoad_detect()
{

}







