#pragma once

#include "stm32f30x.h"


void GPIO_Pin_Init(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode, 
	GPIOSpeed_TypeDef GPIO_Speed, GPIOOType_TypeDef GPIO_OType, GPIOPuPd_TypeDef GPIO_PuPd);

namespace PeripheralInit
{
	enum class Comp1InvInp : uint32_t
	{
		PA0 = COMP_InvertingInput_IO1,
		PA4 = COMP_InvertingInput_DAC1OUT1,
		PA5 = COMP_InvertingInput_DAC1OUT2
	};

	enum class CompMode : uint32_t
	{
		HighSpeed = COMP_Mode_HighSpeed,
		MediumSpeed = COMP_Mode_MediumSpeed,
		LowPower = COMP_Mode_LowPower,
		UltraLowPower = COMP_Mode_UltraLowPower,
	};

	enum class CompOutPol : uint32_t
	{
		Inverted = COMP_OutputPol_Inverted,
		NonInverted = COMP_OutputPol_NonInverted
	};

	/* This function is obsolete, hence SHOULD NOT be called. */
	void InitClock(void);
	void InitGPIO(void);
	void InitEXTI(void);
	/* This function is obsolete, hence SHOULD NOT be called. */
	void InitNVIC(void);
	void InitTIM1(void);
	void InitTIM2(void);
	void InitTIM4(void);
	void InitCOMP1(const Comp1InvInp, const CompOutPol = CompOutPol::NonInverted, const CompMode = CompMode::HighSpeed);
	void EnableCOMP1(void);
	//void SetCOMP1InvInp(const Comp1InvInp);
	//void SetCOMP1OutPol(const CompOutPol);
}


