#include "OpenT12.h"

double PTemp[FixNum] = {TempP1, TempP2, TempP3, TempP4}; //温度拟合系数
long Calibration_Base[FixNum] = {150, 200, 250, 300, 325, 350, 375, 400, 425, 450};
long Calibration_Input[FixNum] = {0};

//显示曲线系数
void ShowCurveCoefficient(void)
{
    Clear();
    char buffer[50];
    for (long i = 0; i < 4; i++)
    {
        sprintf(buffer, "P[%d]=%.8f\n", i, PTemp[i]);
        Disp.setCursor(12, i * 12 + 8);
        Disp.print(buffer);
    }

    while (!sys_KeyProcess())
    {
        Display();
    }

    DrawTempCurve();
}

//绘制温度曲线
void DrawTempCurve(void)
{
    int x;
    int count;
    sys_Counter_Set(0, 63, 1, 0);
    while (!sys_KeyProcess())
    {
        Clear();
        count = sys_Counter_Get();

        //绘制参考文字
        char buffer[20];
        sprintf(buffer, "ADC %d", count * 64);
        DrawHighLightText(128 - Disp.getUTF8Width(buffer) - 2, 36, buffer);

        sprintf(buffer, "温度 %.1lf", CalculateTemp(count * 64, PTemp));
        DrawHighLightText(128 - Disp.getUTF8Width(buffer) - 2, 51, buffer);

        //绘制曲线
        Disp.setDrawColor(2);
        for (int y = 0; y < 64; y++)
        {
            x = map(CalculateTemp(y * 64, PTemp), 0, CalculateTemp(4095, PTemp) + 1, 0, 127);
            Disp.drawPixel(x, 63 - y);

            //画指示针
            if (y == count)
                Draw_Slow_Bitmap(x - 4, 63 - y - 4, PositioningCursor, 8, 8);

        }
        Disp.setDrawColor(1);

        //利用抖动产生灰度 绘制底部参考网格
        if (DisplayFlashTick % 2)
            for (int yy = 0; yy < 64; yy += 8)
                for (int xx = 0; xx < 128; xx += 8) Disp.drawPixel(xx + 2, yy + 4);


        Display();
    }
}

//校准界面
void CalibrationTemperature(void)
{
    Pop_Windows("长按取消校准");
    delay(1000);

    //暂时解除菜单flag，安全机制在菜单开启的时候不允许加热
    Menu_System_State = 0;

    bool ExitCalibration_Flag = false;
    uint8_t key = 0;
    char buffer[20];
    uint16_t SetADC = 0;
    int ADC, LastADC;
    double TmpP[FixNum] = {0.0};
    sys_Counter_Set(0, 4095, 1, 0);
    for (uint8_t i = 0; i < FixNum;)
    {
        Clear();

        SetADC = sys_Counter_Get();

        ADC = GetADC0();
        //加热
        if (ADC != -1)
        {
            LastADC = ADC;

            if (LastADC < SetADC) SetPOWER(255);
            else SetPOWER(0);
        }

        //绘制参考文字

        sprintf(buffer, "目标:%ld°C", Calibration_Base[i]);
        DrawHighLightText(128 - Disp.getUTF8Width(buffer) - 2, 21, buffer);

        sprintf(buffer, "采样ADC %d", LastADC);
        DrawHighLightText(128 - Disp.getUTF8Width(buffer) - 2, 36, buffer);

        sprintf(buffer, "设定ADC：%d", SetADC);
        DrawHighLightText(128 - Disp.getUTF8Width(buffer) - 2, 51, buffer);

        Disp.setDrawColor(2);
        //绘制进度条
        Disp.drawBox(0, 0, map(i, 0, FixNum - 1, 0, 128), 4);
        //绘制曲线
        uint8_t x;
        for (int y = 0; y < 64; y++)
        {
            x = map(CalculateTemp(y * 64, TmpP), 0, CalculateTemp(4095, TmpP) + 1, 0, 127);
            Disp.drawPixel(x, 63 - y);
        }
        Disp.setDrawColor(1);

        Display();

        //处理按键
        key = sys_KeyProcess();
        switch (key)
        {
            case 1:
            case 3:
                delay(50);
                Calibration_Input[i] = SetADC;
                polyfit(i + 1, Calibration_Input, Calibration_Base, 3, TmpP);
                i++;
                break;

            case 2:
                ExitCalibration_Flag = true;
                i = 255;
                break;

            default:
                break;
        }
    }
    //关闭功率管输出
    SetPOWER(0);

    //若中途退出，则不保存
    if (!ExitCalibration_Flag)
    {
        //进行曲线拟合
        polyfit(FixNum, Calibration_Input, Calibration_Base, 3, PTemp);
        Pop_Windows("曲线拟合完成!");
        delay(800);
        ShowCurveCoefficient();
    } else
    {
        Pop_Windows("取消校准");
        delay(1000);
    }


    Menu_System_State = 1;
}

//********** 曲线拟合程序 **********
//曲线拟合算法来至https://blog.csdn.net/m0_37362454/article/details/82456616 by欧阳小俊

/*============================================================
  高斯消元法计算得到 n 次多项式的系数
  n: 系数的个数
  ata: 线性矩阵
  sumxy: 线性方程组的Y值
  p: 返回拟合的结果
  ============================================================*/
void gauss_solve(long n, double A[], double x[], double b[])
{
    long i, j, k, r;
    double max;
    for (k = 0; k < n - 1; k++)
    {
        max = fabs(A[k * n + k]);   // find maxmum
        r = k;
        for (i = k + 1; i < n - 1; i++)
        {
            if (max < fabs(A[i * n + i]))
            {
                max = fabs(A[i * n + i]);
                r = i;
            }
        }
        if (r != k)
        {
            for (i = 0; i < n; i++) //change array:A[k]&A[r]
            {
                max = A[k * n + i];
                A[k * n + i] = A[r * n + i];
                A[r * n + i] = max;
            }
            max = b[k];                  //change array:b[k]&b[r]
            b[k] = b[r];
            b[r] = max;
        }

        for (i = k + 1; i < n; i++)
        {
            for (j = k + 1; j < n; j++)
                A[i * n + j] -= A[i * n + k] * A[k * n + j] / A[k * n + k];
            b[i] -= A[i * n + k] * b[k] / A[k * n + k];
        }
    }

    for (i = n - 1; i >= 0; x[i] /= A[i * n + i], i--)
    {
        for (j = i + 1, x[i] = b[i]; j < n; j++)
            x[i] -= A[i * n + j] * x[j];
    }
}

/*==================polyfit(n,x,y,poly_n,a)===================*/
/*=======拟合y=a0+a1*x+a2*x^2+……+apoly_n*x^poly_n========*/
/*=====n是数据个数 xy是数据值 poly_n是多项式的项数======*/
/*===返回a0,a1,a2,……a[poly_n]，系数比项数多一（常数项）=====*/
void polyfit(long n, long x[], long y[], long poly_n, double p[])
{
    long i, j;
    double* tempx, * tempy, * sumxx, * sumxy, * ata;

    tempx = (double*) calloc(n, sizeof(double));
    sumxx = (double*) calloc((poly_n * 2 + 1), sizeof(double));
    tempy = (double*) calloc(n, sizeof(double));
    sumxy = (double*) calloc((poly_n + 1), sizeof(double));
    ata = (double*) calloc((poly_n + 1) * (poly_n + 1), sizeof(double));
    for (i = 0; i < n; i++)
    {
        tempx[i] = 1;
        tempy[i] = y[i];
    }
    for (i = 0; i < 2 * poly_n + 1; i++)
    {
        for (sumxx[i] = 0, j = 0; j < n; j++)
        {
            sumxx[i] += tempx[j];
            tempx[j] *= x[j];
        }
    }
    for (i = 0; i < poly_n + 1; i++)
    {
        for (sumxy[i] = 0, j = 0; j < n; j++)
        {
            sumxy[i] += tempy[j];
            tempy[j] *= x[j];
        }
    }
    for (i = 0; i < poly_n + 1; i++)
    {
        for (j = 0; j < poly_n + 1; j++)
        {
            ata[i * (poly_n + 1) + j] = sumxx[i + j];
        }
    }
    gauss_solve(poly_n + 1, ata, p, sumxy);

}