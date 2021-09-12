#ifndef MathFun_H
#define MathFun_H

#include "Type.h"

int get_Dec_Bit(int x, int n);
int Get_Dec_Deep(int x);
int Get_Int_Bit_Sum(int x);
uint32_t Get_UTF8_Ascii_Pix_Len(uint8_t size, char* s);
uint32_t UTF8_HMiddle(uint32_t x, uint32_t w, uint8_t size, char* s);
uint32_t Minute2Millis(float min);

void ShowBuffer(uint8_t* buffer, uint32_t size);

//卡尔曼滤波
typedef struct
{
    float LastP;//上次估算协方差 初始化值为0.02
    float Now_P;//当前估算协方差 初始化值为0
    float out;//卡尔曼滤波器输出 初始化值为0
    float Kg;//卡尔曼增益 初始化值为0
    float Q;//过程噪声协方差 初始化值为0.001
    float R;//观测噪声协方差 初始化值为0.543
} KFP;//Kalman Filter parameter
float kalmanFilter(KFP* kfp, float input);

double ESP32_ADC2Vol(uint16_t adc);
int sign(int x);

#endif