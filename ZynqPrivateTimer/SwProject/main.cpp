/**
 * @brief	  	Main software file for using Zynq Private Timer
 * @author		Caglayan DOKME, caglayandokme@gmail.com
 * @date	  	September 26, 2021 -> Created
 */

 /** Libraries **/
#include "xparameters.h"
#include "xscutimer.h"
#include "xscugic.h"
#include <stdio.h>

/** Definitions **/
// The private timer is clocked half the CPU frequency
#define TIMER_1S_VALUE 		(XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2)
#define TIMER_500MS_VALUE	(TIMER_1S_VALUE / 2)
#define TIMER_250MS_VALUE	(TIMER_1S_VALUE / 4)
#define TIMER_100MS_VALUE	(TIMER_1S_VALUE / 10)

/** Hardware Instances **/
XScuTimer timer;
XScuGic gic;

/** Global Variables **/
volatile bool b_timerExpired = false;

void StartTimer()
{
	// Load the value to start down counting
	XScuTimer_LoadTimer(&timer, TIMER_1S_VALUE);

	// Trigger the start
	XScuTimer_Start(&timer);
}

void TimerIrqHandler(void* arguments)
{
	// Raise the flag
	b_timerExpired = true;

	// Restart the timer
	StartTimer();
}

void InitGic()
{
	uint32_t errCode = 0;

	// Find the related configuration
	XScuGic_Config* config = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);

	// Check if the configuration is found
	if(nullptr == config)
		while(1);

	// Initialize the driver using the given configuration
	errCode = XScuGic_CfgInitialize(&gic, config, config->CpuBaseAddress);
	if(XST_SUCCESS != errCode)
		while(1);

	errCode = XScuGic_SelfTest(&gic);
	if(XST_SUCCESS != errCode)
		while(1);

	Xil_ExceptionInit();

	// Connect the IRQ controller handler to the hardware interrupt handling logic
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, Xil_ExceptionHandler(XScuGic_InterruptHandler), &gic);

	// Connect the Timer IRQ handler to the timer interrupt
	XScuGic_Connect(&gic, XPAR_PS7_SCUTIMER_0_INTR, Xil_ExceptionHandler(TimerIrqHandler), &timer);

	// Enable the interrupt from private timer device
	XScuGic_Enable(&gic, XPAR_PS7_SCUTIMER_0_INTR);

	// Enable interrupts on the processor
	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);
}

void InitTimer()
{
	uint32_t errCode = 0;

	// Find the related configuration
	XScuTimer_Config* config = XScuTimer_LookupConfig(XPAR_PS7_SCUTIMER_0_DEVICE_ID);

	// Check if the configuration is found
	if(nullptr == config)
		while(1);

	// Initialize the driver using the given configuration
	errCode = XScuTimer_CfgInitialize(&timer, config, config->BaseAddr);
	if(XST_SUCCESS != errCode)
		while(1);

	// Make a self-test to driver initially
	errCode = XScuTimer_SelfTest(&timer);
	if(XST_SUCCESS != errCode)
		while(1);

	// Enable IRQ generation
	XScuTimer_EnableInterrupt(&timer);
}

int main()
{
	// Initializations
	InitTimer();
	InitGic();

	StartTimer();

	// Application loop
	while(1)
	{
		// Wait for the timer IRQ
		while(!b_timerExpired);
		b_timerExpired = false;

		printf("Timer expired!\n");
	}
}
