The Zynq Triple Timer Counter example design consists of only a Zynq PS.
The main duty of the example application is to utilize the TTC device of the Zynq. An IRQ also included in order to cover the interrupt provided by the TTC device.
The hardware project can be regenerated using the [tickle file](https://github.com/CaglayanDokme/EmbeddedSystemExamples/blob/main/ZynqTripleTimerCounter/HwProject/ZynqTripleTimerCounter.tcl) provided. It is based on Zedboard.
The software project must be regenerated manually. Only the [application codes](https://github.com/CaglayanDokme/EmbeddedSystemExamples/blob/main/ZynqTripleTimerCounter/SwProject/ZynqTtcMain.cpp) has been uploaded to this repo.
Application codes are also based on Zedboard, modify it if you have a different board or component.
