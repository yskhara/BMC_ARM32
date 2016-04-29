#include "interrupt.h"

extern "C"
{
	/******************************************************************************/
	/*            Cortex-M4 Processor Exceptions Handlers                         */
	/******************************************************************************/

	/**
		* @brief  This function handles NMI exception.
		* @param  None
		* @retval None
		*/
	void NMI_Handler(void)
	{
	}

	/**
	  * @brief  This function handles Hard Fault exception.
	  * @param  None
	  * @retval None
	  */
	void HardFault_Handler(void)
	{
		/* Go to infinite loop when Hard Fault exception occurs */
		while (1)
		{
		}
	}

	/**
	  * @brief  This function handles SVCall exception.
	  * @param  None
	  * @retval None
	  */
	void SVC_Handler(void)
	{
	}

	/**
	  * @brief  This function handles PendSVC exception.
	  * @param  None
	  * @retval None
	  */
	void PendSV_Handler(void)
	{
	}

	/**
	  * @brief  This function handles SysTick Handler.
	  * @param  None
	  * @retval None
	  */
	void SysTick_Handler(void)
	{
	}

	void EXTI15_10_IRQHandler(void)
	{
		if (EXTI_GetFlagStatus(EXTI_Line13) == SET)
		{
			StartMotor();
			/* Clear interrupt flag */
			EXTI_ClearFlag(EXTI_Line13);
		}
	}

	/* Handle COMP1 interrupt */
	void COMP1_2_3_IRQHandler(void)
	{
		if (EXTI_GetFlagStatus(EXTI_Line21) == SET)
		{
			COMP1Interrupt();
			/* Clear interrupt flag */
			EXTI_ClearFlag(EXTI_Line21);
		}
	}

	/* Handle TIM4 interrupt */
	void TIM4_IRQHandler(void)
	{
		if (TIM_GetFlagStatus(TIM4, TIM_FLAG_Update) == SET)
		{
			TIM4Interrupt();
			TIM_ClearFlag(TIM4, TIM_FLAG_Update);
		}
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
