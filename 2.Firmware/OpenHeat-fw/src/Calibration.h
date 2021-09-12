#ifndef Calibration_H
#define Calibration_H

#include "Type.h"
//温度拟合
/*
 50-50
100-70
150-134
200-221
250-300
300-379
350-460
400-520
450-579
*/

//温度拟合经验常数：按照实际情况通过拟合得到
#define TempP1 51.6128997803
#define TempP2 0.1443523616
#define TempP3 0.0000061750
#define TempP4 -0.0000000001

#define FixNum 10
extern double PTemp[FixNum]; //温度拟合系数
extern long Calibration_Base[FixNum];
extern long Calibration_Input[FixNum];

void ShowCurveCoefficient(void);
void DrawTempCurve(void);
void CalibrationTemperature(void);
void gauss_solve(long n, double A[], double x[], double b[]);
void polyfit(long n, long x[], long y[], long poly_n, double p[]);
#endif