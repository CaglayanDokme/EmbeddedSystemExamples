The Zynq Triple Timer Counter example design consists of only a Zynq PS.
The main duty of the example application is to utilize the TTC device of the Zynq. An IRQ also included in order to cover the interrupt provided by the TTC device.
The hardware project can be regenerated using the [tickle file](https://github.com/CaglayanDokme/EmbeddedSystemExamples/blob/main/ZynqTripleTimerCounter/HwProject/ZynqTripleTimerCounter.tcl) provided. It is based on Zedboard.
The software project must be regenerated manually. Only the [application codes](https://github.com/CaglayanDokme/EmbeddedSystemExamples/blob/main/ZynqTripleTimerCounter/SwProject/ZynqTtcMain.cpp) has been uploaded to this repo.

Two different timers of a single TTC device used in the example.
* The first timer is responsible for generating periodic events, default 1Hz.
* The second timer is used to generate a PWM signal over JA2 pin of the Zedboard. The window frequency is 1000Hz in default. Details of operation can be found in block comments.
