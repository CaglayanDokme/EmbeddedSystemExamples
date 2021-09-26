/**
 * @file 	main.cpp
 * @brief	Main source file for the Microblaze of the MIG example project.
 * @author	Caglayan DOKME, caglayandokme@gmail.com
 * @date	September 12, 2021 -> Created
 */

/** Libraries **/
#include "xparameters.h"
#include "xuartlite.h"
#include "xgpio.h"
#include "sleep.h"

/** Hardware Instances **/
XGpio switches;
XUartLite uart;

int main()
{
	/** Initialization **/
	if(XUartLite_Initialize(&uart, XPAR_UART_DEVICE_ID) != XST_SUCCESS)
		while(1);

	if(XGpio_Initialize(&switches, XPAR_GPIO_DEVICE_ID) != XST_SUCCESS)
		while(1);

	/** Variables **/
	uint8_t swStatus, prevSwStatus;					// Status of the switches
	uint8_t txBuffer[6] = {0, 0, 0, 0, '\r', '\n'};	// Message to be sent

	while(1)
	{
		swStatus = XGpio_DiscreteRead(&switches, 1);

		// Check if the value changed
		if(swStatus != prevSwStatus)
		{
			prevSwStatus = swStatus;

			// Write the values to corresponding character positions
			// '0' : Conversion to ASCII
			for(size_t idx = 0; idx < 4; ++idx)
				txBuffer[idx] = '0' + (((swStatus & (1 << idx)) == 0) ? 0 : 1);

			XUartLite_Send(&uart, txBuffer, sizeof(txBuffer));
		}
	}

}
