#ifndef LOWPOWERMODE_H
#define LOWPOWERMODE_H

#include "main.h"

#define PWR_WAKEUP_PIN 		PWR_WAKEUP_PIN2  //|PWR_WAKEUP_PIN1

void SleepMode_Measure(void);
void StopMode_Measure(void);
void SYSCLKConfig_STOP(void);
int8_t CheckIsStandby(void);
void StandbyMode(void);
void Sleep_Opt(void);
void SystemIntoStandbyMode(void);
#endif

