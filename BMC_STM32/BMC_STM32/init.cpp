#include "init.h"

void GPIO_Pin_Init(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode,
	GPIOSpeed_TypeDef GPIO_Speed, GPIOOType_TypeDef GPIO_OType, GPIOPuPd_TypeDef GPIO_PuPd)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	//GPIO_StructInit(&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed;
	GPIO_InitStruct.GPIO_OType = GPIO_OType;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd;

	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void PeripheralInit::InitClock(void)
{
	/* TIMx clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);


}

void PeripheralInit::InitGPIO(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

	//GPIO_InitTypeDef GPIO_InitStructure;
	//GPIO_StructInit(&GPIO_InitStructure);

	/* Configure GPIOA */
	GPIO_Pin_Init(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5, GPIO_Mode_AN, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL);
	//GPIO_Pin_Init(GPIOA, GPIO_Pin_5, GPIO_Mode_AF, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL);

	/* Configure GPIOB */
	GPIO_Pin_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL);
	GPIO_Pin_Init(GPIOB, GPIO_Pin_8, GPIO_Mode_AF, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL);
	
	/* Configure GPIOC */
	GPIO_Pin_Init(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2, GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL);
	GPIO_Pin_Init(GPIOC, GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5, GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL);
	GPIO_Pin_Init(GPIOC, GPIO_Pin_13, GPIO_Mode_IN, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_UP);

	/* TIM1_CH1 output port is on PC0(default)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);*/

	// maps alternate functions.
	//GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_1); // TIM2_CH1/TIM2_ETR is on af1
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_8); //COMP1OUT is on af8
														 //GPIO_PinAFConfig(GPIOC, GPIO_PinSource0, GPIO_AF_2); //TIM1CH1 is on af2
}

void PeripheralInit::InitEXTI()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_StructInit(&EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = EXTI_Line21;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure);


	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);

	EXTI_InitStructure.EXTI_Line = EXTI_Line13;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure);
}

void PeripheralInit::InitNVIC()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = COMP1_2_3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void PeripheralInit::InitTIM1()
{
	/* TIMx clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);

	// Time base configuration     72MHz / 1000 = 72kHz
	TIM_TimeBaseStructure.TIM_Period = 999; // count 0-999
	TIM_TimeBaseStructure.TIM_Prescaler = 0; // presc=1
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	// PWM1 Mode configuration: TIM3_CH1   500 / 1000 = 50% duty
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Set;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	TIM_Cmd(TIM1, ENABLE);
}

void PeripheralInit::InitTIM2()
{
	/* TIMx clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);

	// Time base configuration     (72MHz / 72k) / 1000 = 1Hz
	TIM_TimeBaseStructure.TIM_Period = 1000 - 1; // count 0-999
	TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t)(72000 - 1); // presc=72k
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	// PWM1 Mode configuration: TIM2_CH1   500 / 1000 = 50% duty
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Set;
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_CtrlPWMOutputs(TIM2, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}

void PeripheralInit::InitTIM4()
{
	/* TIMx clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

	// Time base configuration     (72MHz / 144) / 65536 = 1kHz
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF; // count 0-65535
	TIM_TimeBaseStructure.TIM_Prescaler = 144 - 1; // presc=36
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_CtrlPWMOutputs(TIM4, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
}

void PeripheralInit::InitCOMP1(const Comp1InvInp cii, const CompOutPol pol, const CompMode mode)
{
	// Note: comparators require no clock input
	COMP_InitTypeDef COMP_InitStruct;
	//COMP_StructInit(&COMP_InitStruct);

	COMP_InitStruct.COMP_InvertingInput		= static_cast<uint32_t>(cii);
	COMP_InitStruct.COMP_OutputPol			= static_cast<uint32_t>(pol);
	COMP_InitStruct.COMP_Mode				= static_cast<uint32_t>(mode);
	COMP_InitStruct.COMP_NonInvertingInput	= COMP_NonInvertingInput_IO1;
	COMP_InitStruct.COMP_Output				= COMP_Output_None;
	COMP_InitStruct.COMP_BlankingSrce		= COMP_BlankingSrce_None;
	COMP_InitStruct.COMP_Hysteresis			= COMP_Hysteresis_No;

	COMP_Init(COMP_Selection_COMP1, &COMP_InitStruct);
}

void PeripheralInit::EnableCOMP1()
{
	COMP_Cmd(COMP_Selection_COMP1, ENABLE);
}

/*
void PeripheralInit::SetCOMP1InvInp(const Comp1InvInp cii)
{
	/*!< Write to COMPx_CSR register *
	*(__IO uint32_t *) (COMP_BASE + COMP1) |= static_cast<uint32_t>(cii);
}

void PeripheralInit::SetCOMP1OutPol(const CompOutPol pol)
{
	/*!< Write to COMPx_CSR register *
	*(__IO uint32_t *) (COMP_BASE + COMP1) |= static_cast<uint32_t>(pol);
}
*/
