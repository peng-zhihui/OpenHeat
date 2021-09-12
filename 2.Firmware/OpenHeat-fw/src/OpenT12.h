#ifndef OPENT12_H
#define OPENT12_H

#include <math.h>
#include <Arduino.h>
#include <Shell.h>
#include <PID_v1.h>
#include <U8g2lib.h>
#include <Ticker.h>
#include <OneButton.h>
#include "BluetoothSerial.h"
#include "Type.h"
#include "serial_putc.h"
#include "MyShell.h"
#include "Rotary.h"
#include "MathFun.h"
#include "ExternDraw.h"
#include "Event.h"
#include "Menu.h"
#include "Calibration.h"
#include "TipControl.h"
#include "Beep.h"
#include "Bezier.h"
#include "TipManager.h"
#include "FilesSystem.h"
#include "qrcode.h"
#include "BLE.h"
#include "Bitmap.h"

#define setbit(x, y)       x|=(1<<y)
#define clrbit(x, y)       x&=~(1<<y)
#define reversebit(x, y)   x^=(1<<y)
#define getbit(x, y)       ((x) >> (y)&1)

//注意Pin36与Pin39连接了内部霍尔传感器,不建议复用高敏感用途
//另外，建议给ADC输入串联一个0.1uF的电容，有利于减少噪声
#define LED_Pin       2
#define BEEP_PIN      25

#define ROTARY_PIN1      27
#define ROTARY_PIN2      14
#define BUTTON_PIN      33

#define TIP_ADC_PIN   -1
#define NTC_ADC       -1
#define POWER_ADC_PIN 4
#define CUR_ADC       -1

#define PWM1_PIN      26
#define PWM2_PIN      -1
#define SW_PIN        -1

//计算主电源电压
#define POWER_ADC_VCC_R1 10000
#define POWER_ADC_R2_GND 1000

#define CNSize 12
#define SCREEN_COLUMN 128
#define SCREEN_ROW    64
#define SCREEN_PAGE_NUM 8
#define SCREEN_FONT_ROW 4

//温度限制
#define TipMaxTemp 300
#define TipMinTemp 0

//烙铁安装移除事件计时器冷却时间：防止事件临界值抖动
#define TipEvent_CoolTime 1500
//到温提示声播放稳定期：防止温度没有稳定下来就播放到温音效
#define TempToneStabilitytime 1000

enum SYSLANG
{
    LANG_English = 0,
    LANG_Chinese,
    LANG_Russian,
};

enum PANELSET
{
    PANELSET_Simple = 0,
    PANELSET_Detailed,
};

enum HANDLETRIGGER
{
    HANDLETRIGGER_VibrationSwitch = 0,
    HANDLETRIGGER_ReedSwitch,
};

enum MESSAGETYPE
{
    LOG_INFO = 0,
    LOG_OK,
    LOG_FAILED,
    LOG_WARNING,
    LOG_ERROR,
};

void Log(MESSAGETYPE type, char* s);
void SYS_Reboot(void);
void About(void);


extern uint64_t ChipMAC;
extern char ChipMAC_S[19];
extern char CompileTime[20];

extern BluetoothSerial SerialBT;
extern OneButton RButton;
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C Disp;
extern PID MyPID;
extern KFP KFP_Temp;

extern uint8_t DEBUG_MODE;

extern char* TipName;

extern float BootTemp;
extern float SleepTemp;
extern float ScreenProtectorTime;
extern float BoostTemp;

extern float ShutdownTime;
extern float SleepTime;
extern float BoostTime;

extern bool SYS_Ready;

extern bool TipInstallEvent;
extern bool TipCallSleepEvent;
extern bool TempToneFlag;
extern bool SleepScreenProtectFlag;

extern bool ERROREvent;
extern bool ShutdownEvent;
extern bool SleepEvent;
extern bool BoostEvent;
extern bool UnderVoltageEvent;

extern bool PWM_WORKY;


extern uint8_t PIDMode;
extern uint8_t Use_KFP;

extern uint8_t PanelSettings;
extern uint8_t ScreenFlip;
extern uint8_t SmoothAnimation_Flag;
extern float ScreenBrightness;
extern uint8_t OptionStripFixedLength_Flag;

extern uint8_t Volume;
extern uint8_t RotaryDirection;
extern uint8_t HandleTrigger;

extern double SYS_Voltage;
extern float UndervoltageAlert;
extern char BootPasswd[20];
extern uint8_t Language;
extern uint8_t MenuListMode;

extern float ADC_PID_Cycle;

extern double Get_MainPowerVoltage(void);

enum TEMP_CTRL_STATUS_CODE
{
    TEMP_STATUS_ERROR = 0,
    TEMP_STATUS_OFF,
    TEMP_STATUS_SLEEP,
    TEMP_STATUS_BOOST,
    TEMP_STATUS_WORKY,
    TEMP_STATUS_HEAT,
    TEMP_STATUS_HOLD,
};
extern uint8_t TempCTRL_Status;
extern char* TempCTRL_Status_Mes[];


#endif