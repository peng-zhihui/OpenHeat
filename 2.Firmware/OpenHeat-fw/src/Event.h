#ifndef SYSEvent_H
#define SYSEvent_H

#include "Type.h"

extern uint32_t BoostTimer;

extern uint32_t EventTimerUpdate;
void TimerUpdateEvent(void);

void BoostButton_EventLoop(void);
void FastPID_MenuSummon_EventLoop(void);
void ShutdownEventLoop(void);

bool EnterPasswd(void);
void SetPasswd(void);

void TimerEventLoop(void);
void SYS_StateCode_Update(void);

void SW_IRQHandler(void);
void SW_WakeLOOP(void);
#endif