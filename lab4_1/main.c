#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"
#include "driverlib/fpu.h"
#include "utils/uartstdio.h"


uint32_t ui32ADC0Value[4]; //create an array that will be used for storing the data read from the ADC FIFO
volatile uint32_t ui32Avg1;
volatile uint32_t ui32Avg2,ui32Value1,ui32Value2;
uint32_t ui32ADC1Value[4];

int main(void)
{

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);//enable ADC0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);//enable ADC1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);	// UART 0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);	// ADC
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);	// UART
	ADCHardwareOversampleConfigure(ADC0_BASE, 64);
	ADCHardwareOversampleConfigure(ADC1_BASE, 64);

	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);//configure the ADC sequencer. We want to use ADC0, sample sequencer 1,
	//we want the processor to trigger the sequence and we want to use the highest priority.
	ADCSequenceConfigure(ADC1_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH7);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH7);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH7);
	ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_CH7|ADC_CTL_IE|ADC_CTL_END);

	ADCSequenceStepConfigure(ADC1_BASE, 1, 0, ADC_CTL_CH6);
	ADCSequenceStepConfigure(ADC1_BASE, 1, 1, ADC_CTL_CH6);
	ADCSequenceStepConfigure(ADC1_BASE, 1, 2, ADC_CTL_CH6);
	ADCSequenceStepConfigure(ADC1_BASE,1,3,ADC_CTL_CH6|ADC_CTL_IE|ADC_CTL_END);

	ADCSequenceEnable(ADC0_BASE, 1); //enable ADC sequencer 1 of ADC0
	ADCSequenceEnable(ADC1_BASE, 1); //enable ADC sequencer 1 of ADC1


	while(1)
	{
		ADCIntClear(ADC0_BASE, 1);//ADC interrupt status flag
		ADCIntClear(ADC1_BASE, 1);

		ADCProcessorTrigger(ADC0_BASE, 1); //trigger the ADC conversion with software
		ADCProcessorTrigger(ADC1_BASE, 1);

		while(!ADCIntStatus(ADC0_BASE, 1, false) && !ADCIntStatus(ADC1_BASE, 1, false))
		{
		}

		ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);//read the ADC value from the ADC Sample Sequencer 1 FIFO
		ADCSequenceDataGet(ADC1_BASE, 1, ui32ADC1Value);//read the ADC value from the ADC Sample Sequencer 1 FIFO
		ui32Avg1 = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
		ui32Value1 = ((100 * ui32Avg1) / 4096);
		ui32Avg2 = (ui32ADC1Value[0] + ui32ADC1Value[1] + ui32ADC1Value[2] + ui32ADC1Value[3] + 2)/4;
		ui32Value2 = ((100 * ui32Avg2) / 4096);
	}


}


