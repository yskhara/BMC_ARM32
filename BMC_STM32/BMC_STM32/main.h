#pragma once

#define __attribute__(x) 
//#include "../Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_include.h"

#include "stm32f30x.h"
#include "system_stm32f30x.h"

#include "stdio.h"

#include "init.h"
#include "interrupt.h"

extern uint32_t SystemCoreClock;

enum class MainMsg
{
	StartMotor,
	StopMotor,
	CompInterrupt
};

int main(void);

void StartMotor(void);
void StopMotor(void);
void COMP1Interrupt(void);
void TIM4Interrupt(void);

//static void COMP1_2_3_IRQHandler(void) __attribute__((interrupt("IRQ")));
