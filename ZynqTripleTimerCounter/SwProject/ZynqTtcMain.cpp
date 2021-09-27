/**
 * @brief	Main software file for the Zynq Triple Timer Counter example project.
 * @author	Caglayan DOKME, caglayandokme@gmail.com
 * @date	September 27, 2021 -> Created
 */

/** Libraries **/
#include "xparameters.h"
#include "xscugic.h"		// Global Interrupt Controller
#include "xttcps.h"			// Triple Timer Counter
#include <stdio.h>

/** Definitions **/
#define TTC0_FREQ_HZ		1

/** Custom Structures **/
struct TmrCntrSetup{
	uint32_t 	frequency 	= 0;
	uint16_t 	interval	= 0;
	uint16_t 	options		= 0;
	uint8_t 	prescaler	= 0;
};

/** Hardware Instances **/
XTtcPs 	timerTtc0;
XScuGic gic;

/** Global Variables **/
TmrCntrSetup timerTtc0Setup;
volatile bool b_timerTtc0Expired = false;

void TimerIrqHandler(void* arguments)
{
	// Handler mismatch check
	if(&timerTtc0 != arguments)
		return;

	const uint32_t irqStatus = XTtcPs_GetInterruptStatus(&timerTtc0);

	if((irqStatus & XTTCPS_IXR_INTERVAL_MASK) != 0)
	{
		XTtcPs_ClearInterruptStatus(&timerTtc0, XTTCPS_IXR_INTERVAL_MASK);

		b_timerTtc0Expired = true;
	}
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

	// Connect the TTC0 interrupt handler to the related interrupt
	XScuGic_Connect(&gic, XPS_TTC0_0_INT_ID, Xil_ExceptionHandler(TimerIrqHandler), &timerTtc0);

	// Enable the interrupt from TTC0 device
	// This line only allows interrupts from TTC0 device
	// You must explicitly configure the TTC0 device to generate interrupts
	XScuGic_Enable(&gic, XPS_TTC0_0_INT_ID);

	// Enable interrupts on the processor
	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);
}

void InitTimerTtc0()
{
	uint32_t errCode = 0;

	// Find the related configuration
	XTtcPs_Config* config = XTtcPs_LookupConfig(XPAR_PS7_TTC_0_DEVICE_ID);

	// Check if the configuration is found
	if(nullptr == config)
		while(1);

	// Initialize the driver using the given configuration
	errCode = XTtcPs_CfgInitialize(&timerTtc0, config, config->BaseAddress);
	if(XST_SUCCESS != errCode)
		while(1);

	// Self-test initially
	errCode = XTtcPs_SelfTest(&timerTtc0);
	if(XST_SUCCESS != errCode)
		while(1);

	timerTtc0Setup.frequency 	= TTC0_FREQ_HZ;
	timerTtc0Setup.options 		= XTTCPS_OPTION_INTERVAL_MODE | XTTCPS_OPTION_WAVE_DISABLE;

	errCode = XTtcPs_SetOptions(&timerTtc0, timerTtc0Setup.options);
	if(XST_SUCCESS != errCode)
		while(1);

	// Let the driver calculate required interval and prescaler values using the given frequency
	XTtcPs_CalcIntervalFromFreq(&timerTtc0, timerTtc0Setup.frequency, &timerTtc0Setup.interval, &timerTtc0Setup.prescaler);

	// Upon a failure during calculations, the values are set to their highest possible limit
	if((XTTCPS_MAX_INTERVAL_COUNT == timerTtc0Setup.interval) || (0xFF == timerTtc0Setup.prescaler))
		while(1);

	// Set interval and prescaler using the given values
	XTtcPs_SetInterval(&timerTtc0, timerTtc0Setup.interval);
	XTtcPs_SetPrescaler(&timerTtc0, timerTtc0Setup.prescaler);

	// Enable interrupt generation (GIC also needs to be configured)
	XTtcPs_DisableInterrupts(&timerTtc0, XTTCPS_IXR_ALL_MASK);
	XTtcPs_EnableInterrupts(&timerTtc0, XTTCPS_IXR_INTERVAL_MASK);

	/* Details of Calculation:
	 * The TTC device can either use the PS CLK or an external clock from PL
	 * The input frequency can be seen from the XTtcPs_Config instance.
	 *
	 * Inside the device, the clock is first divided by the prescaler.
	 * Mathematical representation is as follows where the P is the prescaler value
	 *
	 *		PRESCALED_CLK = CLK / (2 ^ (P + 1));
	 *
	 * Secondly, the scaled clock is given to the internal counter.
	 * The internal counter counts up to(down from) the given interval value.
	 * If enabled, an interrupt signal can be generated at each overflow(underflow) of the counter.	 */
}

int main()
{
	// Setup the system
	InitTimerTtc0();
	InitGic();

	// Start the timer
	XTtcPs_Start(&timerTtc0);

	// Application loop
	while(1)
	{
		// Wait for timer flag
		while(!b_timerTtc0Expired);
		b_timerTtc0Expired = false;

		// Log the event (Should occur with TTC0_FREQ_HZ frequency)
		printf("Event!\n");
	}
}
