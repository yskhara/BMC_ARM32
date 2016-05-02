#include "main.h"

uint32_t _clocks_in_us;

void delay_us(unsigned int delay)
{
	unsigned int t = delay * _clocks_in_us;
	for (unsigned int i = 0; i < t; i++)
	{
		__NOP();
	}
}



void blink()
{
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
	delay_us(10000);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
}


/*
 * stage|| 0 | 1 | 2 | 3 | 4 | 5 |  lo /  hi / fb
 * -----++---+---+---+---+---+---+
 *    U || H | H | Z | L | L | Z | PC0 / PC3 / PA0
 *    V || L | Z | H | H | Z | L | PC1 / PC4 / PA4
 *    W || Z | L | L | Z | H | H | PC2 / PC5 / PA5
 *
 *    H ||PC3|PC3|PC4|PC4|PC5|PC5|
 *    L ||PC1|PC2|PC2|PC0|PC0|PC1|
 */

static const uint8_t PhasePerRev = 6;

static const uint16_t DrivePhasePattern_All = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;

static const uint16_t DrivePhasePatterns[PhasePerRev] =
{
	GPIO_Pin_1 | GPIO_Pin_3,
	GPIO_Pin_2 | GPIO_Pin_3,
	GPIO_Pin_2 | GPIO_Pin_4,
	GPIO_Pin_0 | GPIO_Pin_4,
	GPIO_Pin_0 | GPIO_Pin_5,
	GPIO_Pin_1 | GPIO_Pin_5,
};

static const uint16_t DrivePhasePatternsXored[PhasePerRev] =
{
	DrivePhasePattern_All ^ DrivePhasePatterns[0],
	DrivePhasePattern_All ^ DrivePhasePatterns[1],
	DrivePhasePattern_All ^ DrivePhasePatterns[2],
	DrivePhasePattern_All ^ DrivePhasePatterns[3],
	DrivePhasePattern_All ^ DrivePhasePatterns[4],
	DrivePhasePattern_All ^ DrivePhasePatterns[5],
};

static const uint8_t initRevCount = 100;

static const uint16_t initRevTime[initRevCount] = 
{
	550,
	500,
	450,
	400,
	350,
	300,
	250,
	200,
	150,
	100,
};

// The value must be kept in a range of 0 to 5.
static uint8_t DrivePhase = 0;

void StartMotor(void)
{
	printf("Entering hard commutating stage.\n");

	NVIC_DisableIRQ(COMP1_2_3_IRQn);
	NVIC_DisableIRQ(TIM4_IRQn);

	DrivePhase = 0;

	for (int i = 0; i < initRevCount; i++)
	{
		for (int j = 0; j < PhasePerRev; j++)
		{
			// Init position
			GPIO_ResetBits(GPIOC, DrivePhasePattern_All ^ DrivePhasePatterns[j]);
			GPIO_SetBits(GPIOC, DrivePhasePatterns[j]);
			//delay_us(initRevTime[i]);
			delay_us(200);
		}
	}

	GPIO_SetBits(GPIOB, GPIO_Pin_0);

	printf("sc\n");

	/*
	 * Okay, these character code stuff is killing me. Ugh!
	 * COMP1 interrupt is triggered on a rising edge of COMP1 output.
	 * COMP1割り込みは，COMP1出力の立ち上がりエッジでのみ発生する．
	 * The neutral point is connected to the NON-inverted input of COMP1; so if COMP1 output polarity is set to non-inverted (default), 
	 * 中点電位はCOMP1の非反転入力に接続されるので，COMP1の出力極性設定が非反転（既定）のとき，
	 * COMP1 output would fall on the moment when the voltage of one pole goes above the neutral voltage.
	 * あるピンの電位が中点電位を超える瞬間にCOMP1の出力は立ち下がる．
	 * Hence, COMP1 output polarity must be set to inverted to receive COMP1 interrupt on the moment when one pole goes above neutral.
	 * 従って，あるピンの電位が中点電位より高くなる瞬間を捉えるにはCOMP1の出力極性設定を反転にすればよく，
	 * the polarity must be set to non-inverted otherwise.
	 * 逆に，あるピンの電位が中点電位より低くなる瞬間を捉えるにはCOMP1の出力極性設定を非反転にすればよい．
	 */

	/*
	 * On Phase5, rising edge on the U-Pole is expected; so output polarity must be set to inverted.
	 * Phase5においては，U相（PA0）の立ち上がりエッジを期待する．よって，出力極性は反転．
	 */
	PeripheralInit::InitCOMP1(PeripheralInit::Comp1InvInp::PA0, PeripheralInit::CompOutPol::Inverted);

	TIM_SetCounter(TIM4, 0);

	NVIC_EnableIRQ(COMP1_2_3_IRQn);
}

void StopMotor(void)
{

}

inline void Commutate()
{
	// maybe I could just throw an exception for DrivePhase > 5.
	if (DrivePhase >= 5)
	{
		DrivePhase = 0;
	}
	else
	{
		DrivePhase++;
	}

	GPIO_ResetBits(GPIOC, DrivePhasePatternsXored[DrivePhase]);
	GPIO_SetBits(GPIOC, DrivePhasePatterns[DrivePhase]);
}

void COMP1Interrupt(void)
{
	printf("c1i\n");
	NVIC_DisableIRQ(COMP1_2_3_IRQn);

	// TODO: Implement speed control; manipulate the counter value
	TIM_CounterModeConfig(TIM4, TIM_CounterMode_Down);

	NVIC_EnableIRQ(TIM4_IRQn);
}

/*
 * Once again: 
 * stage|| 0 | 1 | 2 | 3 | 4 | 5 |  lo /  hi / fb
 * -----++---+---+---+---+---+---+
 *    U || H | H | Z | L | L | Z | PC0 / PC3 / PA0
 *    V || L | Z | H | H | Z | L | PC1 / PC4 / PA4
 *    W || Z | L | L | Z | H | H | PC2 / PC5 / PA5
 *
 *    H ||PC3|PC3|PC4|PC4|PC5|PC5|
 *    L ||PC1|PC2|PC2|PC0|PC0|PC1|
 */


void TIM4Interrupt(void)
{
	printf("t4i\n");
	NVIC_DisableIRQ(TIM4_IRQn);

	// DrivePhase should be updated in this function call: 
	Commutate();

	switch (DrivePhase)
	{
	case 0:
		PeripheralInit::InitCOMP1(PeripheralInit::Comp1InvInp::PA5, PeripheralInit::CompOutPol::NonInverted);
		break;
	case 1:
		PeripheralInit::InitCOMP1(PeripheralInit::Comp1InvInp::PA4, PeripheralInit::CompOutPol::Inverted);
		break;
	case 2:
		PeripheralInit::InitCOMP1(PeripheralInit::Comp1InvInp::PA0, PeripheralInit::CompOutPol::NonInverted);
		break;
	case 3:
		PeripheralInit::InitCOMP1(PeripheralInit::Comp1InvInp::PA5, PeripheralInit::CompOutPol::Inverted);
		break;
	case 4:
		PeripheralInit::InitCOMP1(PeripheralInit::Comp1InvInp::PA4, PeripheralInit::CompOutPol::NonInverted);
		break;
	case 5:
		PeripheralInit::InitCOMP1(PeripheralInit::Comp1InvInp::PA0, PeripheralInit::CompOutPol::Inverted);
		break;
	default:
		printf("drivephase outta range");
		// Again, an exception can be thrown.
		break;
	}

	/*
	* Phase0,2,4‚É‚¨‚¢‚Ä‚ÍCŠe‘Š‚Ì—§‚¿‰º‚ª‚èƒGƒbƒW‚ðŠú‘Ò‚·‚éD‚æ‚Á‚ÄCo—Í‹É«‚Í”ñ”½“]D
	*/
	//PeripheralInit::InitCOMP1(PeripheralInit::Comp1InvInp::PA5, PeripheralInit::CompOutPol::NonInverted);

	TIM_CounterModeConfig(TIM4, TIM_CounterMode_Up);
	TIM_SetCounter(TIM4, 0);

	NVIC_EnableIRQ(COMP1_2_3_IRQn);
}

int main(void)
{
	//SystemInit();

	_clocks_in_us = SystemCoreClock / 1000000;

	PeripheralInit::InitGPIO();
	PeripheralInit::InitEXTI();

	//NVIC_SetPriority(COMP1_2_3_IRQn, 0x0F);
	//NVIC_EnableIRQ(COMP1_2_3_IRQn);
	NVIC_SetPriority(EXTI0_IRQn, 0x0F);
	NVIC_EnableIRQ(EXTI15_10_IRQn);

	PeripheralInit::InitTIM1();
	PeripheralInit::InitTIM2();
	PeripheralInit::InitTIM4();

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

	PeripheralInit::InitCOMP1(PeripheralInit::Comp1InvInp::PA0);
	PeripheralInit::EnableCOMP1();

	blink();
	printf("Initialization completed.\n");

	const uint32_t time = 100;

	while (1)
	{
	}

	/*
	* stage|| 1 | 2 | 3 | 4 | 5 | 6 |  lo /  hi
	* -----++---+---+---+---+---+---+
	*    U || H | H | Z | L | L | Z | PC0 / PC3
	*    V || L | Z | H | H | Z | L | PC1 / PC4
	*    W || Z | L | L | Z | H | H | PC2 / PC5
	* 
	*    H ||PC3|PC3|PC4|PC4|PC5|PC5|
	*    L ||PC1|PC2|PC2|PC0|PC0|PC1|
	*/

	// init position
	GPIO_ResetBits(GPIOB, GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
	GPIO_SetBits(GPIOB, GPIO_Pin_4);
	TIM_SetCompare3(TIM3, 0);
	TIM_SetCompare1(TIM3, 500);

	while (1)
	{
		// 1
		//GPIO_ResetBits(GPIOB, GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
		//GPIO_SetBits(GPIOB, GPIO_Pin_4);
		TIM_SetCompare3(TIM3, 0);
		TIM_SetCompare1(TIM3, 500);

		//delay_us(500000);
	}

	return 0;
}
