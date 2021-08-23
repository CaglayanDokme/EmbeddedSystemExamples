/**
 * @file 	main.c
 * @brief	Main source file for the first Cortex-A9 of Shared BRAM Example platform.
 * @author	Caglayan DOKME, caglayandokme@gmail.com
 * @date	August 22, 2021 -> Created
 */

/*** Libraries ***/
#include "xparameters.h"	// System related parameters (produced by Vitis based on the provided XSA or HDF)
#include "xgpio.h"		// BSP of the Xilinx AXI GPIO Controller
#include "sleep.h"		// Time based operations

/*** Hardware Instances ***/
XGpio leds;

int main()
{
	/** System Setup **/
	// Initialize the GPIO controller of switches
	if(XGpio_Initialize(&leds, XPAR_LEDS_DEVICE_ID) != XST_SUCCESS)
		while(1);

	// Current switch value will be stored here
	uint8_t* switchValue = (uint8_t*)XPAR_BRAM_0_BASEADDR;

	/** Application Loop **/
	while(1)
	{
		// Read the switch status, and use it to set the LEDs
		XGpio_DiscreteWrite(&leds, 1, *switchValue);

		// 1ms delay
		usleep(1000);
	}
}
