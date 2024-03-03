# AVR-microPC
AVR based microcomputer outputing VGA

The code is entirely write in Assembly for better performance from the MCU

AVR-PC Specs :

* MCU as GPU and CPU : ATmega16A (16K ROM, 1K RAM, 0.5K EEPROM)  at 16MHz
* One CO-CPU as IO expander and for higher storage capacity : ATmega16A (16K ROM, 1K RAM, 0.5K EEPROM)  at 16MHz
* SRAM as VRAM : 128Kb (2x8Ko to make 16Bits colors)
* VGA Output : 128x64 @16bit, 60Hz

I was inspired a lot from Ben Eater, mainly for the VGA protcol.
His website : https://eater.net/vga

# The programs

| <b>NAME</b>    |<b>TYPE</b>| <b>DESCRIPTION</b>                                                                   |
|:---------------|:---------:|:-------------------------------------------------------------------------------------|
| Blink          | asm       | bliks an output (for testing only)                                                   |
| ButtonColor    | asm       | different buttons will dispay differents color on the screen                         |
| ColorSync      | asm       | display an annimation of colors                                                      |
| Fade           | asm       | Fade red to black, then blue to black, and Green to black, then repeat               |
| HelloWorld     | asm       | Displays "HELLO LEO LOL" on the screen                                               |
| Main copy      | asm       | some test with the RAM (NWY)                                                         |
| Renderer8bit   | asm       | a 3d renderer that can draws custom cubes in real time, all in asm                   |
| Main           | asm       | an other 3d renderer that can draws custom cubes in real time, using 16 fixed points |
| Main_C1/C2     | asm       | some test to make the 2 CPU communicate and display a 16Ko+ image on the screen (NWY)|
| Snake          | asm       | some test for a snake game (NWY)                                                     |
| UART           | asm       | Uart communication beetwen the AVR-microPC and a computer                            |
| import Image   | py        | image converter                                                                      |

(NWY) = not working yet

# C compiling

The folder C_compiler include all necessary stuff to compile C code for the microPC
the asm.S file has some basics functions made for this PC
the MainC.cpp file has some code example like raytracer and 3D engines build for this PC
    
### How to use it 
    
- send command :

    To compile or send your code the the microPC, simply type <br>```send <-Optimisation> <Cfile path> <Sfile path> <Compile (-c) or send (-s)>```<br>
    In this case, you would compile with<br> ```send -O3 MainC.cpp asm.S -c```<br> or compile and send the code to the microPC with<br> ```send -O3 MainC.cpp asm.S -s```<br>
    <U> WARNING</U> : You need to have the avr-gcc (and avr-g++) compiler installed and in your PATH, and you need to have the avrdude installed and configure the Run.bat file with your own path configuration
    <br> For more information, type ```send -h``` or ```send help```