
#include  <stdint.h>
#include  <stdbool.h>
#include  "inc/tm4c123gh6pm.h"
#include  "inc/hw_memmap.h"
#include  "inc/hw_types.h"
#include  "driverlib/sysctl.h"
#include  "driverlib/interrupt.h"
#include  "driverlib/gpio.h"
#include  "driverlib/timer.h"
#include "inc/hw_gpio.h"

uint32_t count=0;
uint32_t sw1press=0;
uint32_t  ui32Period;

void setup(void)       // set crystal freq and enable GPIO pins
{
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}


void led_pin_config(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
}

void enable_timer(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); //TIMER0_BASE is the start of the timer registers for Timer0 in the peripheral section of the memory map.
	TimerConfigure (TIMER0_BASE, TIMER_CFG_PERIODIC);

	ui32Period = ( SysCtlClockGet () / 10) / 2; // to get 10 Hz period and divide that by two, since we want a count that is ½ of that for the interrupt.
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);//Subtract one from the timer period since the interrupt fires at the zero count.

	IntEnable(INT_TIMER0A); //master interrupt enable API for all interrupts. Enable first 16 bit timer register
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);//enables a specific event within the timer to generate an interrupt. Here on time out of TIMER0A
	IntMasterEnable();

	TimerEnable(TIMER0_BASE, TIMER_A); //enable the timer
}

int  main( void)

{

	setup();
	led_pin_config();

	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0,GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4,GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);

	enable_timer();

	while (1)
	{
	}
}

void  detectkeypress ( void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)==0x0)
	{
		sw1press++;
		if(sw1press==0x2)
		{
			count++;
			if(count%3==1)
			{

				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,2);
			}
			else if(count%3==2)
			{

				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,4);

			}
			else if(count>0 && count%3==0)
			{

				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,8);

			}

		}
		else if( GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)==0x1)
		{
			sw1press=0;
		}

	}
	else
	{
		sw1press=0;
	}

}
