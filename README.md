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
    - resistance in the MOSFET gate cirquit to limit the pulse current through MCU output
    - allow TH componets usage, reuse some items from original board
    - IP pins can be assigned better to make less croddings on the PCB
    - sensor button connections can be done better. but analog curcit rules should be taken into account
    - as I plan to etch teh single-layer board at home, I will use tow heatsinks for LM780x instead of the PCB polygons. There 
        is a lot of space under the PCB.
* Programming UART pins and reset button on the board. Pro Mini hasn't USB socket.
* Review the firmware
    - make it interactive
    - all initially predefined values can be set by UART
    - keep configuration values in the FLASH

# Arduino Power

Almost all Arduino boards has an LDO onboard. This LDO allows up to 16V input and can supply arduino and external cirquit 
with the +3.3V/+5V <150mA power according to the Arduino version. All the MCU ATMEGA328 allow both +3.3V and +5V power. The 
difference is that on lower voltage Arduino works stable with less crystal frequency. That is why +3.3V has 8MHz crystal and 
+5V has 16MHz crystal. Any version of the Arduino Pro Mini can safely be supplied with the +5V.

LM393 can operate with power in +2V..+36V range, means it can be perfectly powered from any version of the Arduino. It 
cosumes <20mA, this current is a maximal sink current and should be lower than that.

Unfortunately, my Arduino LDO is broken because of the previous experiments, that is why I kept the external LDO here and lead 
the +5V to the Arduino Vcc input. Vcc - is a direct way to teh Arduino power excluding on-baord LDO.


