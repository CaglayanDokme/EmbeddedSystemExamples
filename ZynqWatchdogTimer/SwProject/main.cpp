/**
 * @brief	  	Main software file for using Zynq Watchdog Timer
 * @author		Caglayan DOKME, caglayandokme@gmail.com
 * @date	  	September 26, 2021 -> Created
 */

 /** Libraries **/
#include "xparameters.h"
#include "xgpiops.h"
#include "xscugic.h"
#include "xscuwdt.h"
#include <stdio.h>

/** Definitions **/
// The watchdog timer is clocked half the CPU frequency
#define WDT_TIMER_1S_VALUE 		(XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2)
#define WDT_TIMER_500MS_VALUE	(WDT_TIMER_1S_VALUE / 2)
#define WDT_TIMER_250MS_VALUE	(WDT_TIMER_1S_VALUE / 4)
#define WDT_TIMER_100MS_VALUE	(WDT_TIMER_1S_VALUE / 10)

#define PIN_BTN8	50

#define TYPE_INPUT	0
#define TYPE_OUTPUT	1

#define ENABLE		1
#define DISABLE		0

#define	HIGH 		1
#define	LOW 		0

/** Hardware Instances **/
XGpioPs gpio;
XScuGic gic;
XScuWdt watchdog;

/** Global Variables **/
volatile bool b_btnIrqFlag = false;

void GpioIrqHandler(void* arguments)
{
	if(HIGH == XGpioPs_ReadPin(&gpio, PIN_BTN8))
		b_btnIrqFlag = true;
}

void WatchdogIrqHandler(void* arguments)
{
	printf("Watchdog expired!\n");
}

void InitGpio()
{
	uint32_t errCode = 0;

	// Find the related configuration
	XGpioPs_Config* config = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);

	// Check if the configuration is found
	if(nullptr == config)
		while(1);

	// Initialize the driver using the given configuration
	errCode = XGpioPs_CfgInitialize(&gpio, config, config->BaseAddr);
	if(XST_SUCCESS != errCode)
		while(1);

	errCode = XGpioPs_SelfTest(&gpio);
	if(XST_SUCCESS != errCode)
		while(1);

	// Declare input pins (JE7, JE8, JE9, JE10)
	XGpioPs_SetDirectionPin(&gpio, PIN_BTN8,  TYPE_INPUT);

	// Declare pins that will generate interrupt
	XGpioPs_SetIntrTypePin(&gpio, PIN_BTN8, XGPIOPS_IRQ_TYPE_EDGE_RISING);

	// Declare a callback function for the PS GPIO interrupt
	XGpioPs_SetCallbackHandler(&gpio, &gpio, (XGpioPs_Handler)GpioIrqHandler);

	// Enable the interrupt generation of PS GPIO
	XGpioPs_IntrEnablePin(&gpio, PIN_BTN8);
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

	// Connect the PS GPIO device driver handler to the PS GPIO interrupt
	XScuGic_Connect(&gic, XPS_GPIO_INT_ID, Xil_ExceptionHandler(XGpioPs_IntrHandler), &gpio);

	// Connect the Watchdog timer IRQ handler to the related interrupt
	XScuGic_Connect(&gic, XPS_SCU_WDT_INT_ID,(Xil_ExceptionHandler)WatchdogIrqHandler, &watchdog);

	// Enable the interrupt from PS GPIO device
	XScuGic_Enable(&gic, XPS_GPIO_INT_ID);

	// Enable interrupts on the processor
	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);
}

void InitWatchdog()
{
	uint32_t errCode = 0;

	// Find the related configuration
	XScuWdt_Config* config = XScuWdt_LookupConfig(XPAR_PS7_SCUWDT_0_DEVICE_ID);

	// Check if the configuration is found
	if(nullptr == config)
		while(1);

	// Initialize the driver using the given configuration
	errCode = XScuWdt_CfgInitialize(&watchdog, config, config->BaseAddr);
	if(XST_SUCCESS != errCode)
		while(1);

	errCode = XScuWdt_SelfTest(&watchdog);
	if(XST_SUCCESS != errCode)
		while(1);

	// Setup the timer as watch-dog timer
	XScuWdt_SetWdMode(&watchdog);

	// Load the value
	XScuWdt_LoadWdt(&watchdog, WDT_TIMER_1S_VALUE * 5);

	// Start the timer
	XScuWdt_Start(&watchdog);
}

int main()
{
	// Initialize the system
	InitWatchdog();
	InitGpio();
	InitGic();

	// Check if the system had been reset due to
	// Watchdog expiration or a normal power-up
	if(XScuWdt_IsWdtExpired(&watchdog))
		printf("System had been reset due to Watchdog!\n");
	else
		printf("System powered up normally..\n");

	// Application loop
	while(1)
	{
		// Wait until the button is pressed
		while(!b_btnIrqFlag);
		b_btnIrqFlag = false;

		// Restart the Watchdog timer so that it doesn't expire
		XScuWdt_RestartWdt(&watchdog);

		printf("Button pressed!\n");
	}
}
