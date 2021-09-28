/**
 * @brief	Main software file for the Zynq Triple Timer Counter example project.
 * @author	Caglayan DOKME, caglayandokme@gmail.com
 * @date	September 27, 2021 -> Created
 * 			September 28, 2021 -> PWM signal generation added.
 *
 */

/** Libraries **/
#include "xparameters.h"
#include "xscugic.h"		// Global Interrupt Controller
#include "xttcps.h"			// Triple Timer Counter
#include <stdio.h>

/** Definitions **/
#define TTC0_FREQ_HZ		1
#define TTC1_FREQ_HZ		(1 * 1000)

/** Custom Structures **/
struct TmrCntrSetup{
	uint32_t 	frequency 	= 0;
	uint16_t 	interval	= 0;
	uint16_t 	options		= 0;
	uint8_t 	prescaler	= 0;
};

struct TmrPwmSetup : TmrCntrSetup{
	uint16_t matchValue = 0;

	void CalcSetMatchValue(float dutyCycle)
	{
		if(0 == interval)
			while(1);

		matchValue = uint16_t(float(interval) * dutyCycle);
	}
};

/** Hardware Instances **/
XTtcPs 	timerTtc0,	// Periodic event generation
		timerTtc1;	// PWM signal generation
XScuGic gic;

/** Global Variables **/
TmrCntrSetup 	timerTtc0Setup;
TmrPwmSetup		timerTtc1Setup;
volatile bool 	b_timerTtc0Expired = false;

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

void InitTimerTtc1()
{

	uint32_t errCode = 0;

	// Find the related configuration
	XTtcPs_Config* config = XTtcPs_LookupConfig(XPAR_PS7_TTC_1_DEVICE_ID);

	// Check if the configuration is found
	if(nullptr == config)
		while(1);

	// Initialize the driver using the given configuration
	errCode = XTtcPs_CfgInitialize(&timerTtc1, config, config->BaseAddress);
	if(XST_SUCCESS != errCode)
		while(1);

	// Self-test initially
	errCode = XTtcPs_SelfTest(&timerTtc1);
	if(XST_SUCCESS != errCode)
		while(1);

	timerTtc1Setup.frequency 	= TTC1_FREQ_HZ;
	timerTtc1Setup.options 		= XTTCPS_OPTION_INTERVAL_MODE | XTTCPS_OPTION_MATCH_MODE | XTTCPS_OPTION_WAVE_POLARITY;

	// Apply selected options
	errCode = XTtcPs_SetOptions(&timerTtc1, timerTtc1Setup.options);
	if(XST_SUCCESS != errCode)
		while(1);

	// Let the driver calculate required interval and prescaler values using the given frequency
	XTtcPs_CalcIntervalFromFreq(&timerTtc1, timerTtc1Setup.frequency, &timerTtc1Setup.interval, &timerTtc1Setup.prescaler);

	// Upon a failure during calculations, the values are set to their highest possible limit
	if((XTTCPS_MAX_INTERVAL_COUNT == timerTtc1Setup.interval) || (0xFF == timerTtc1Setup.prescaler))
		while(1);

	// Set interval and prescaler using the given values
	XTtcPs_SetInterval(&timerTtc1, timerTtc1Setup.interval);
	XTtcPs_SetPrescaler(&timerTtc1, timerTtc1Setup.prescaler);

	// Calculate a PWM match value using the duty cycle
	timerTtc1Setup.CalcSetMatchValue(0.63);
	if(0 == timerTtc1Setup.matchValue)
		while(1);

	// Apply the calculated match value
	XTtcPs_SetMatchValue(&timerTtc1, 0, timerTtc1Setup.matchValue);
	if(XTtcPs_GetMatchValue(&timerTtc1, 0) != timerTtc1Setup.matchValue)
		while(1);

	/* Details of Operation:
	 * Each timer in each TTC device has three different match values.
	 * Each match value can be used to generate interrupts.
	 * In timer1 of the TTC0 device, we use the match 0 to generate a PWM signal.
	 *
	 * The match 0 is special as it has the ability to generate a waveform if the
	 * XTTCPS_OPTION_WAVE_POLARITY is marked at TTC device options.
	 * The other match values doesn't have this feature.
	 *
	 * Frequency represents the PWM window frequency.
	 * Interval is the resolution of a single PWM window.
	 * Lastly, the match value is equal to the (dutyCycle * interval) */
}

int main()
{
	// Setup the system
	InitTimerTtc0();
	InitTimerTtc1();
	InitGic();

	// Start the timers
	XTtcPs_Start(&timerTtc0);
	XTtcPs_Start(&timerTtc1);

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
