#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"

/*




 * Function Name: setup()

 * Input: none

 * Output: none

 * Description: Set crystal frequency and enable GPIO Peripherals

 * Example Call: setup();
 */

void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}


void led_pin_config(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
}

//uint8_t ui8LED = 0;  // declaring global variable
uint8_t sw1count= 0;



int main(void)
{



	//uint8_t ui8PinData=2;

	setup();
	led_pin_config();
	HWREG(GPIO_PORTF_BASE+GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE+GPIO_O_CR) |= GPIO_PIN_0;

	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4);

	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);


	while(1)
	{
		if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == 0)
		{
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,2 );
			SysCtlDelay(8000000);
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,4 );
			SysCtlDelay(8000000);
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,8 );
			SysCtlDelay(8000000);
		}

		else // (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == 1)
		{
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0 );
		}


		if( GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0 && (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == 1) )
		{
			sw1count ++;
			//while(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == 0 && (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == 1) )
			//{
			while (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0)
				{GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,8 );
				if (sw1count%3 == 0)
					{SysCtlDelay(6700000);}
				else if (sw1count%3 == 1)
					{SysCtlDelay(6700000);
					SysCtlDelay(6700000);}
				else
					{SysCtlDelay(6700000);
					SysCtlDelay(6700000);
					SysCtlDelay(6700000);}
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,8 );}
			//}

		}

	}
}
