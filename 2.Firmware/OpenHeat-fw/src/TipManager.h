#ifndef TIPMANAGER_H
#define TIPMANAGER_H

#include "Type.h"

//最大烙铁头配置数量
#define MaxTipConfig 10
extern uint8_t TipID;       //当前选择的烙铁头
extern uint8_t TipTotal;    //有效烙铁头配置总数

struct TIPCONFIG
{
    double PTemp[4];    //温度系数
    float PID[2][3];   //PID系数{远PID，近PID}
    char name[20];    //名称
};

extern TIPCONFIG MyTip[MaxTipConfig];

void LoadTipConfig(void);
void SaveTipConfig(void);

void FlashTipMenu(void);

void NewTipConfig(void);
void TipRename(void);
void TipDel(void);

#endif