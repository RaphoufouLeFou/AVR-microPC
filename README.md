# AVR-microPC
AVR based microcomputer outputing VGA

The code is entirely write in Assembly for better performance from the MCU

AVR-PC Specs :

* MCU as GPU and CPU : ATmega16A (16K ROM, 2K RAM, 0.5K EEPROM)  at 16MHz
* SRAM as VRAM : 128Kb (2x8Ko to make 16Bits colors)
* VGA Output : 128x64 @16bit, 60Hz

I was inspired a lot from Ben Eater, mainly for the VGA protcol.
His website : https://eater.net/vga
