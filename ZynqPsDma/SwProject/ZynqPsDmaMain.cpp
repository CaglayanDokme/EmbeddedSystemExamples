/**
 * @brief	  	Main software file for using Zynq PS DMA
 * @author		Caglayan DOKME, caglayandokme@gmail.com
 * @date	  	October 6, 2021 -> Created
 */

/** Libraries **/
#include "xparameters.h"
#include "xdmaps.h"
#include "xscugic.h"
#include "xil_cache.h"

/** Hardware Instances **/
XDmaPs 	dma;
XScuGic gic;

/** Global Variables **/
uint8_t sourceBuffer[128] 	= {0};
uint8_t destBuffer[128] 	= {0};

/** IRQ Flags **/
volatile bool b_dmaDone = false;

/** Function Declarations **/
void InitDma();		// DMA Initialization
void InitGic();		// GIC Initialization
void DmaDoneHandler(void* arguments);				// DMA Done IRQ Handler
void DmaFaultHandler(void* arguments);				// DMA Fault IRQ Handler
void StartDma(void* src, void* dest, size_t size);	// Function to start the DMA

int main()
{
	// Disable the cache to prevent coherency issues
	Xil_DCacheDisable();

	// Prepare incremental data
	for(uint8_t idx = 0; idx < sizeof(sourceBuffer); sourceBuffer[idx] = idx++);

	// Initialization
	InitGic();
	InitDma();

	// Start
	StartDma(sourceBuffer, destBuffer, sizeof(sourceBuffer));

	// Application loop
	while(1)
	{
		// Wait until the transfer is completed
		while(!b_dmaDone);
		b_dmaDone = false;

		// Compare the data buffers
		if(memcmp(sourceBuffer, destBuffer, sizeof(sourceBuffer)) != 0)
			while(1);

		// Re-adjust the source buffer
		for(uint8_t idx = 0; idx < sizeof(sourceBuffer); ++sourceBuffer[idx++]);

		// Restart the DMA
		StartDma(sourceBuffer, destBuffer, sizeof(sourceBuffer));
	}
}

void DmaDoneHandler(void* arguments)
{
	b_dmaDone = true;
}

void DmaFaultHandler(void* arguments)
{
	// If a fault occurs, stall the application completely
	while(1);
}

void InitDma()
{
	uint32_t errCode = 0;

	// Find the related configuration
	XDmaPs_Config* config = XDmaPs_LookupConfig(XPAR_XDMAPS_1_DEVICE_ID);

	if(nullptr == config)
		while(1);

	// Initialize the DMA instance with given configuration
	errCode = XDmaPs_CfgInitialize(&dma, config, config->BaseAddress);
	if(XST_SUCCESS != errCode)
		while(1);

	// Self test initially
	errCode = XDmaPs_SelfTest(&dma);
	if(XST_SUCCESS != errCode)
		while(1);

	// Connect user IRQ handler to the BSP's IRQ handler
	errCode = XDmaPs_SetDoneHandler(&dma, 0, XDmaPsDoneHandler(DmaDoneHandler), nullptr);
	if(XST_SUCCESS != errCode)
		while(1);

	// Connect user IRQ handler to the BSP's IRQ handler
	errCode = XDmaPs_SetFaultHandler(&dma, XDmaPsDoneHandler(DmaFaultHandler), nullptr);
	if(XST_SUCCESS != errCode)
		while(1);
}

void StartDma(void* src, void* dest, size_t size)
{
	uint32_t errCode = 0;

	// DMA Command structure is used to configure the transaction
	XDmaPs_Cmd dmaCmd{};

	/* !!!!!!!!!!!! ATTENTION !!!!!!!!!!!!
	 * Somehow, the DMA command instance needs to be cleared.
	 * You can either do it during construction or explicitly
	 * using a memset(..) call.
	 * Otherwise, in the background, the generated DMA program
	 * fails and the DMA engine cannot transfer data properly.
	 * Even runtime faults can occur during the operation. */
	// memset(&dmaCmd, 0, sizeof(XDmaPs_Cmd));

	dmaCmd.ChanCtrl.SrcBurstSize 	= 4;
	dmaCmd.ChanCtrl.SrcBurstLen 	= 4;
	dmaCmd.ChanCtrl.SrcInc 			= 1;
	dmaCmd.ChanCtrl.DstBurstSize 	= 4;
	dmaCmd.ChanCtrl.DstBurstLen	 	= 4;
	dmaCmd.ChanCtrl.DstInc 			= 1;
	dmaCmd.BD.SrcAddr 	= (uint32_t) src;
	dmaCmd.BD.DstAddr 	= (uint32_t) dest;
	dmaCmd.BD.Length	= size;

	errCode = XDmaPs_Start(&dma, 0, &dmaCmd, 0);
	if(XST_SUCCESS != errCode)
		while(1);
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

	// Connect the DMA Fault Handler to the related interrupt
	XScuGic_Connect(&gic, XPAR_XDMAPS_0_FAULT_INTR, Xil_ExceptionHandler(XDmaPs_FaultISR), &dma);

	// Connect the DMA Done Handlers to the related interrupts
	XScuGic_Connect(&gic, XPAR_XDMAPS_0_DONE_INTR_0, Xil_ExceptionHandler(XDmaPs_DoneISR_0), &dma);

	// Enable the interrupts from PS DMA device
	// This line only allows interrupts from the PS DMA device
	// You must explicitly configure the PS DMA device to generate interrupts
	XScuGic_Enable(&gic, XPAR_XDMAPS_0_DONE_INTR_0);
	XScuGic_Enable(&gic, XPAR_XDMAPS_0_FAULT_INTR);

	// Enable interrupts on the processor
	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);
}
