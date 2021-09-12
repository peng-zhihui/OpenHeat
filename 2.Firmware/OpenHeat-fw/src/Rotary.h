#ifndef sys_Counter_H
#define sys_Counter_H

#include <stdint.h>
#include "Type.h"

#define ROTARY_TYPE 2

extern uint8_t SYSKey;
extern bool Counter_LOCK_Flag;

void sys_RotaryInit(void);
void sys_Counter_Set(double min, double max, double step, double c);
void sys_Counter_SetVal(double c);
void RotaryUp(void);
void RotaryDown(void);

void sys_Counter_click(void);
void sys_Counter_doubleclick(void);
void sys_Counter_longclick(void);
void sys_Counter_IRQHandler(void);
double sys_Counter_Get(void);
uint8_t sys_Counter_Change(void);

void Clear_RButton_FIFO(void);
uint8_t sys_KeyProcess(void);
#endif