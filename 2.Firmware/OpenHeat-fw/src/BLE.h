#ifndef BLE_H
#define BLE_H

#include "Type.h"

extern char BLE_name[20];
extern uint8_t BLE_State;

void BLE_Init(void);
void BLE_Restart(void);
void BLE_Rename(void);

#endif