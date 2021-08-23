/**
 * @file 	mainMicroblaze.c
 * @brief	Main source file for the Microblaze of Shared BRAM Example system.
 * @author	Caglayan DOKME, caglayandokme@gmail.com
 * @date	August 22, 2021 -> Created
 */

/*** Libraries ***/
#include "xparameters.h"	// System related parameters (produced by Vitis based on the provided XSA or HDF)
#include "xgpio.h"			// BSP of the Xilinx AXI GPIO Controller
#include "sleep.h"			// Time based functions

/*** Hardware Instances ***/
XGpio switches;

int main()
{
	/** System Setup **/
	// Initialize the GPIO controller of switches
	if(XGpio_Initialize(&switches, XPAR_SWITCHES_DEVICE_ID) != XST_SUCCESS)
		while(1);

	// Current switch value will be stored here
	uint8_t* switchValue = (uint8_t*)XPAR_BRAM_0_BASEADDR;

	/** Application Loop **/
	while(1)
	{
		// Read and store the current value of the switches
		*switchValue = XGpio_DiscreteRead(&switches, 1);

		// 1ms delay
		usleep(1000);
	}
}
