# ir_controller
IR controller made with Raspberry Pi Pico
# Schematic
Connect a Raspberry Pi Pico to the other parts as follows. Note that the value of resistor connected to IR-LED is between 10 to 150 ohms depending upon the LED performance.
![schematic.png](shematic.png)
# IR codes
The 32 bit IR codes can be obtained by using [MachiKania](https://github.com/machikania/phyllosoma/tree/production) with [IR_RX class](https://github.com/machikania/lib/tree/master/LIB/IR_RX). See the [help.txt](https://github.com/machikania/lib/blob/master/LIB/IR_RX/help.txt) as an example code to get 32 bit values.
# Buttons
Maximum 25 buttons can be used. Current code and schematic is an example using seven buttons.
# License
Public domain.
