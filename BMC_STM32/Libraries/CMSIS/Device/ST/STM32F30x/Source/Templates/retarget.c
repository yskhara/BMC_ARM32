#include "stm32f30x.h"

void retarget_init()
{
	// Initialize ITM, not UART
	
}

void delay_us(unsigned int delay);

int _write(int fd, char *ptr, int len)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
	delay_us(10000);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
	/* Write "len" of char from "ptr" to file id "fd"
	 * Return number of char written.
	 * Need implementing with UART here. */
	int i = 0;
	for (i = 0; i < len; i++)
		ITM_SendChar((*ptr++));
	return len;
}

int _read (int fd, char *ptr, int len)
{
  /* Read "len" of char to "ptr" from file id "fd"
   * Return number of char read.
   * Need implementing with UART here. */
  return len;
}

void _ttywrch(int ch) {
  /* Write one char "ch" to the default console
   * Need implementing with UART here. */
}

/* SystemInit will be called before main 
void SystemInit()
{
    retarget_init();
}
*/
