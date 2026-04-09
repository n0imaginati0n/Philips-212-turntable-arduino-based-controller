# Philips-212-turntable-arduino-based-controller

A controller upgrade for the classic Philips 212 Hi-Fi turntable.
Replaces all touch switch, motor control, and auto-off functions with an Arduino Uno clone

Files include Arduino sketch and KiCad files

Fork of the

[lens42 project](https://github.com/lens42/Philips-212-turntable-arduino-based-controller)

[Details:](https://lensprojects.wordpress.com/2019/01/08/philips-212-turntable-controller-upgrade/)

# Improvements

* Use of the Ardino Pro Mini
* Self-made PCB. There are not so many parts there, it can be etched at home
* review schematics
    - add necessay loading on the LM780x. According to the datasheet, minimal current is 5..10 mA.
    - LM780x don't like to work on big capacitance load - they need protection diodes
    - small filtering caps on the sensors input to filter noise. input cirquit can be taken from original GA212 schematics
    - ...
* Programming UART pins and reset button on the board. Pro Mini hasn't USB socket.
* Review the firmware
    - make it interactive
    - all initially predefined values can be set by UART
    - keep configuration values in the FLASH