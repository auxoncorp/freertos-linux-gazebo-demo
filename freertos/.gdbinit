target remote :3333

break vApplicationMallocFailedHook
break vApplicationStackOverflowHook
break vAssertCalled

monitor start
