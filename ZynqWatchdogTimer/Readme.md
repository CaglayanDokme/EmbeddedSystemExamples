The Zynq Watchdog Timer example design consists of only a Zynq PS.
The main duty of the example application is to utilize the Watchdog Timer of the Zynq. An IRQ also included in order to cover the interrupt provided by the timer device.
The hardware project can be regenerated using the tickle file provided. It is based on Zedboard.
The software project must be regenerated manually. Only the application codes has been uploaded to this repo.
The application project is responsible of resetting the watchdog timer every time the user presses the BTN8 on the Zedboard. Otherwise, the watchdog expires and resets the system. The reset information can be seen on terminal. For details, inspect the codes
Note that the [boot image](https://github.com/CaglayanDokme/EmbeddedSystemExamples/blob/main/ZynqWatchdogTimer/SwProject/BOOT.bin) is also uploaded as while debugging there is no way to recover the system after a reset caused by Watchdog timer. Using the boot image provided, you can program a non-volatile memory and reload the program again immediately after the reset. 
