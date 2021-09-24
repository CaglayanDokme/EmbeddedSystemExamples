/**
 * @brief	  Main software file for using XADC monitor on Zedboard
 * @author	Caglayan DOKME, caglayandokme@gmail.com
 * @date	  September 24, 2021 -> Created
 */

/** Libraries **/
#include "xparameters.h"
#include "xadcps.h"
#include "sleep.h"
#include <stdio.h>

/** Driver Instances **/
XAdcPs adc;

/**
 * @brief	Initializes the XADC component
 */
void InitXadc()
{
	uint32_t errCode = 0;

	// Find the hardware configuration for XADC component
	XAdcPs_Config* config = XAdcPs_LookupConfig(XPAR_PS7_XADC_0_DEVICE_ID);

	// Check if the configuration found successfully
	if(nullptr == config)
		while(1);

	// Initialize the driver using the given configuration
	errCode = XAdcPs_CfgInitialize(&adc, config, XPAR_PS7_XADC_0_BASEADDR);
	if(XST_SUCCESS != errCode)
		while(1);

	// Test the component initially
	errCode = XAdcPs_SelfTest(&adc);
	if(XST_SUCCESS != errCode)
		while(1);

	// Sequencer must be in the SAFE mode before enabling any of the channels
	XAdcPs_SetSequencerMode(&adc, XADCPS_SEQ_MODE_SAFE);

	// Enable requested channels
	errCode = XAdcPs_SetSeqChEnables(&adc, 	XADCPS_SEQ_CH_TEMP 		| XADCPS_SEQ_CH_VCCINT 	| XADCPS_SEQ_CH_VCCAUX 	|
											XADCPS_SEQ_CH_VBRAM 	| XADCPS_SEQ_CH_VCCPINT | XADCPS_SEQ_CH_VCCPAUX |
											XADCPS_SEQ_CH_VCCPDRO 	| XADCPS_SEQ_CH_VPVN);

	if(XST_SUCCESS != errCode)
		while(1);

	// Disable all alarms
	XAdcPs_SetAlarmEnables(&adc, 0);

	// Sequencer can be activated after enabling the required channels
	XAdcPs_SetSequencerMode(&adc, XADCPS_SEQ_MODE_CONTINPASS);
}

int main()
{
	// Initialization of the XADC component
	InitXadc();

	// Real values of monitored sensors
	float vccInt = 0, vccAux = 0, vBram = 0, vccPint = 0, vccPaux = 0, vddR = 0, vpvn = 0, temp = 0;

	// Application loop
	while(1)
	{
		// Read all monitored values and convert them to actual values
		temp	= XAdcPs_RawToTemperature(XAdcPs_GetAdcData(&adc, XADCPS_CH_TEMP));
		vccInt 	= XAdcPs_RawToVoltage(XAdcPs_GetAdcData(&adc, XADCPS_CH_VCCINT)	);
		vccAux 	= XAdcPs_RawToVoltage(XAdcPs_GetAdcData(&adc, XADCPS_CH_VCCAUX)	);
		vBram 	= XAdcPs_RawToVoltage(XAdcPs_GetAdcData(&adc, XADCPS_CH_VBRAM)	);
		vddR 	= XAdcPs_RawToVoltage(XAdcPs_GetAdcData(&adc, XADCPS_CH_VCCPDRO));
		vccPint = XAdcPs_RawToVoltage(XAdcPs_GetAdcData(&adc, XADCPS_CH_VCCPINT));
		vccPaux = XAdcPs_RawToVoltage(XAdcPs_GetAdcData(&adc, XADCPS_CH_VCCPAUX));
		vpvn 	= XAdcPs_RawToVoltage(XAdcPs_GetAdcData(&adc, XADCPS_CH_VPVN)	);

		// Somehow the converted value is non-calibrated
		vpvn 	= (vpvn / 1.88) * 0.64;

		// Some of the values can be monitored via serial port
		printf("VCC INT: %f\r\n", 	vccInt);
		printf("VP-VN: %f\r\n", 	vpvn);
		printf("\n\r");

		sleep(1);
	}

	// Suppress the unused variable warning
	(void) vccInt;
	(void) vccAux;
	(void) vBram;
	(void) vccPint;
	(void) vccPaux;
	(void) vddR;
	(void) vpvn;
	(void) temp;
}
