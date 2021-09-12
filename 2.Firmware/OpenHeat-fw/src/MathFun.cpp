#include "OpenT12.h"

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C Disp;
/*
	@作用 获取数值某一位
	@传入 数值 指定位数
	@传出 数值指定位数的十进制值
*/
int get_Dec_Bit(int x, int n)
{
    return (int) (x / pow(10.0, n)) % 10;
}

/*
	@作用 获取数值长度
	@传入 数值
	@传出 数值长度
*/
int Get_Dec_Deep(int x)
{
    int i = 0;
    do
    {
        i++;
        x /= 10;
    } while (x != 0);
    return i;
}

/*
	@作用 获取数字每一位的和
	@传入 数字
	@输出 数字每一位的和
*/
int Get_Int_Bit_Sum(int x)
{
    int sum = 0;
    for (int i = 0; i < Get_Dec_Deep(x); i++)
        sum += get_Dec_Bit(x, i);
    return sum;
}


void GetUtf8Length(char const* str, int* cnt)
{
    while (*str != '\0')
    {
        if (*str & 1 << 7)
        {/* 第8位(从右到左)为1*/
            if (*str & (1 << 6))
            {/*第7位为1*/
                if (*str & (1 << 5))
                {/*第6位为1*/
                    if (*str & (1 << 4))
                    {/*第5位为1  11110xxx*/
                        cnt[4]++, cnt[0]++, str += 4;/*4字节的字符总数加1,各种长度总数加1,指针向后移动4位*/
                        continue;/*开始下次循环*/
                    }

                    cnt[3]++, cnt[0]++, str += 3;/* 1110xxxx*/
                    continue;

                }

                cnt[2]++, cnt[0]++, str += 2;/*110xxxxx*/
                continue;

            }
        }
        cnt[1]++, cnt[0]++, str += 1;/*0xxxxxxx*/
        continue;
    }
}

uint32_t Get_UTF8_Ascii_Pix_Len(uint8_t size, char* s)
{
    return Disp.getUTF8Width(s);
}

/*
    @作用 UTF8混合字符串计算水平居中
    @输入：UTF8字符串
    @输出：居中位置
*/
uint32_t UTF8_HMiddle(uint32_t x, uint32_t w, uint8_t size, char* s)
{
    return x + (w - Get_UTF8_Ascii_Pix_Len(size, s)) / 2;
}

//串口打印内存
void ShowBuffer(uint8_t* buffer, uint32_t size)
{
    uint8_t AsciiBuffer[17];
    uint32_t OutputSize = 0;
    for (uint32_t i = 0; i < size; i++)
    {
        if (i % 16 == 0)
        {
            memcpy(AsciiBuffer, buffer + i, 16);
            AsciiBuffer[16] = '\0';
            printf("\n0X%08X | ", i + &(*buffer));
            for (uint8_t j = 0; j < 16; j++)
            {
                OutputSize++;
                if (OutputSize > size) break;
                printf("%02X ", buffer[i + j]);
            }

            printf("| %s", AsciiBuffer);
        }
    }
    printf("\n\n\n%s", buffer);
}


/**
 *卡尔曼滤波器
 *@param KFP *kfp 卡尔曼结构体参数
 *   float input 需要滤波的参数的测量值（即传感器的采集值）
 *@return 滤波后的参数（最优值）
 */
float kalmanFilter(KFP* kfp, float input)
{
    //预测协方差方程：k时刻系统估算协方差 = k-1时刻的系统协方差 + 过程噪声协方差
    kfp->Now_P = kfp->LastP + kfp->Q;
    //卡尔曼增益方程：卡尔曼增益 = k时刻系统估算协方差 / （k时刻系统估算协方差 + 观测噪声协方差）
    kfp->Kg = kfp->Now_P / (kfp->Now_P + kfp->R);
    //更新最优值方程：k时刻状态变量的最优值 = 状态变量的预测值 + 卡尔曼增益 * （测量值 - 状态变量的预测值）
    kfp->out = kfp->out + kfp->Kg * (input - kfp->out);//因为这一次的预测值就是上一次的输出值
    //更新协方差方程: 本次的系统协方差付给 kfp->LastP 威下一次运算准备。
    kfp->LastP = (1 - kfp->Kg) * kfp->Now_P;
    return kfp->out;
}

/*** 
 * @description: 把ESP32的原生ADC采样数据通过经验公式转换为电压
 * @param {uint16_t} adc
 * @return 电压(单位:V)
 */
#define ADC1_MinVol 0.128
#define ADC1_MaxVol 3.11
double ESP32_ADC2Vol(uint16_t adc)
{
    //return ((adc / 4095.0) * (ADC1_MaxVol - ADC1_MinVol)) + ADC1_MinVol;
    return (adc / 4095.0) * 3.3;
}

/*** 
 * @description: 分钟转毫秒
 * @param {uint32_t} min
 * @return {*}
 */
uint32_t Minute2Millis(float min)
{
    return min * 60 * 1000;
}
/*** 
 * @description: 判断符号正负
 * @param {int} x
 * @return {*}
 */
int sign(int x)
{
    return (x > 0) - (x < 0);
}