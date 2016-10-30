#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"

#define PWM_FREQUENCY 55
uint32_t  ui32Period;
volatile uint8_t ui8Adjust;
//volatile uint8_t AdjustGreen;
//volatile uint8_t AdjustBlue;
//	AdjustRed = 240;
//	AdjustBlue = 0;
//	AdjustGreen = 0;

void setup(void)       // set crystal freq and enable GPIO pins
{
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}


void led_pin_config(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
}

/*void enable_timer(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); //TIMER0_BASE is the start of the timer registers for Timer0 in the peripheral section of the memory map.
	TimerConfigure (TIMER0_BASE, TIMER_CFG_PERIODIC);

	ui32Period = ( SysCtlClockGet () / 10) / 2; // to get 10 Hz period and divide that by two, since we want a count that is ½ of that for the interrupt.
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);//Subtract one from the timer period since the interrupt fires at the zero count.

	IntEnable(INT_TIMER0A); //master interrupt enable API for all interrupts. Enable first 16 bit timer register
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);//enables a specific event within the timer to generate an interrupt. Here on time out of TIMER0A
	IntMasterEnable();

	TimerEnable(TIMER0_BASE, TIMER_A); //enable the timer
}*/
/*
 * main.c
 */
int main(void)
{
	volatile uint32_t ui32Load;
	volatile uint32_t ui32PWMClock;
	ui8Adjust=80;
	setup();
	led_pin_config();

	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

	GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0);
	GPIOPinConfigure(GPIO_PD0_M1PWM0);


	//The first three lines below unlock the GPIO commit control register, the fourth
	//configures PF0 & 4 as inputs and the fifth configures the internal pull-up resistors on
	//both pins. The drive strength setting is merely a place keeper and has no function for an	input.
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	//enable_timer();
	//The PWM clock is SYSCLK/64 (set in step 12 above). Divide the PWM clock by the
	//desired frequency (55Hz) to determine the count to be loaded into the Load register. Then
	//subtract 1 since the counter down-counts to zero.Configure module 1 PWM generator 2 and 3
	//as a down-counter and load the count value.
	ui32PWMClock = SysCtlClockGet() / 64;
	ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;

	PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, ui32Load);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000);//PWM Load value is divided by 1000 (which determines the minimum
	//resolution for the servo) and the multiplied by the adjusting value. These numbers could
	//be changed to provide more or less resolution.

	PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);	//PWM module 1,generator 2 enabled as an output and enabled to run.
	PWMGenEnable(PWM1_BASE, PWM_GEN_0);


	while(1)
	{

		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)
		{
			SysCtlDelay(100000);
			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)
			{
				ui8Adjust=ui8Adjust-5;
				if (ui8Adjust < 50)
				{
					ui8Adjust = 50;
				}
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000);
				while(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00);
			}
		}


		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00)
		{
			SysCtlDelay(100000);
			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00)
			{
				ui8Adjust=ui8Adjust+5;
				if (ui8Adjust > 110)
				{
					ui8Adjust = 110;
				}
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000);
				while(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00);
			}

			SysCtlDelay(100000);
		}

	}
}


