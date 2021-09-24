/**
 * @brief	  Main software file for using Zynq PS GPIO on Zedboard
 * @author	Caglayan DOKME, caglayandokme@gmail.com
 * @date	  September 24, 2021 -> Created
 */
  
 /** Libraries **/
#include "xparameters.h"
#include "xgpiops.h"
#include "sleep.h"
#include <stdio.h>

/** Definitions **/
#define PIN_JE1		13
#define PIN_JE2		10
#define PIN_JE3		11
#define PIN_JE4		12
#define PIN_JE7		0
#define PIN_JE8		9
#define PIN_JE9		14
#define PIN_JE10	15

#define TYPE_INPUT	0
#define TYPE_OUTPUT	1

#define ENABLE		1
#define DISABLE		0

#define	HIGH 		1
#define	LOW 		0

/** Hardware Instances **/
XGpioPs gpio;

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

	// Declare output pins (JE1, JE2, JE3, JE4)
	XGpioPs_SetDirectionPin(&gpio, PIN_JE1, TYPE_OUTPUT);
	XGpioPs_SetOutputEnablePin(&gpio, PIN_JE1, ENABLE);

	XGpioPs_SetDirectionPin(&gpio, PIN_JE2, TYPE_OUTPUT);
	XGpioPs_SetOutputEnablePin(&gpio, PIN_JE2, ENABLE);

	XGpioPs_SetDirectionPin(&gpio, PIN_JE3, TYPE_OUTPUT);
	XGpioPs_SetOutputEnablePin(&gpio, PIN_JE3, ENABLE);

	XGpioPs_SetDirectionPin(&gpio, PIN_JE4, TYPE_OUTPUT);
	XGpioPs_SetOutputEnablePin(&gpio, PIN_JE4, ENABLE);

	// Declare input pins (JE7, JE8, JE9, JE10)
	XGpioPs_SetDirectionPin(&gpio, PIN_JE7,  TYPE_INPUT);
	XGpioPs_SetDirectionPin(&gpio, PIN_JE8,  TYPE_INPUT);
	XGpioPs_SetDirectionPin(&gpio, PIN_JE9,  TYPE_INPUT);
	XGpioPs_SetDirectionPin(&gpio, PIN_JE10, TYPE_INPUT);
}

void UpdateOutput(const uint8_t value)
{
	// Each pin is LOW by default
	XGpioPs_WritePin(&gpio, PIN_JE1, LOW);
	XGpioPs_WritePin(&gpio, PIN_JE2, LOW);
	XGpioPs_WritePin(&gpio, PIN_JE3, LOW);
	XGpioPs_WritePin(&gpio, PIN_JE4, LOW);

	switch(value)
	{
		case 1:
			XGpioPs_WritePin(&gpio, PIN_JE1, HIGH);
			break;

		case 2:
			XGpioPs_WritePin(&gpio, PIN_JE2, HIGH);
			break;

		case 3:
			XGpioPs_WritePin(&gpio, PIN_JE3, HIGH);
			break;

		case 4:
			XGpioPs_WritePin(&gpio, PIN_JE4, HIGH);
			break;

		default:
			break;
	}
}

void LogInput()
{
	// Read each pin and log their values
	if(XGpioPs_ReadPin(&gpio, PIN_JE7))
		printf("JE7 is HIGH");
	else
		printf("JE7 is LOW");

	printf("\r\n");

	if(XGpioPs_ReadPin(&gpio, PIN_JE8))
		printf("JE8 is HIGH");
	else
		printf("JE8 is LOW");

	printf("\r\n");

	if(XGpioPs_ReadPin(&gpio, PIN_JE9))
		printf("JE9 is HIGH");
	else
		printf("JE9 is LOW");

	printf("\r\n");

	if(XGpioPs_ReadPin(&gpio, PIN_JE10))
		printf("JE10 is HIGH");
	else
		printf("JE10 is LOW");

	printf("\r\n\r\n");
}

int main()
{
	// Initialize the GPIO driver
	InitGpio();

	// Application loop
	while(1)
	{
		// Activate output pins in turn
		for(uint8_t i = 1; i <= 4; ++i)
		{
			usleep(250 * 1000);	// 250ms
			UpdateOutput(i);
		}

		// LOG the input pins over terminal
		LogInput();
	}
}
