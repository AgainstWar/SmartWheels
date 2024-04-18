#include "stm32f10x.h"
#include "pwm.h"
int main(void)
{
	PWM_Init();
	while (1)
	{
		PWM_SetDutyCycle(1, 500);
	}
}
