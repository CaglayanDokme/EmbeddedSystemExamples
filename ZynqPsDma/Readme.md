The Zynq PS DMA example design consists of only a Zynq PS.
The main duty of the example application is to utilize the DMA device of the Zynq. An set of IRQs also included in order to cover the interrupt provided by the DMA device.
The hardware project can be regenerated using the [tickle file](https://github.com/CaglayanDokme/EmbeddedSystemExamples/blob/main/ZynqPsDma/HwProject/ZynqPsDma.tcl) provided. It is based on Zedboard.
The software project must be regenerated manually. Only the [application codes](https://github.com/CaglayanDokme/EmbeddedSystemExamples/blob/main/ZynqPsDma/SwProject/ZynqPsDmaMain.cpp) has been uploaded to this repo.

The application software configures the first channel of the DMA so that it transfers data from memory to memory.
