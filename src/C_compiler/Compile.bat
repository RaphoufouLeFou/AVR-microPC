avr-g++ -c -Os -w -mmcu=ATmega16 -DF_CPU=16000000L arduinoCppCode.cpp -o outC.o
avr-g++ -c -Os -w -mmcu=ATmega16 -DF_CPU=16000000L isrs.S -o outS.o

avr-gcc -Os -Wl,--gc-sections -mmcu=atmega16 outC.o outS.o -o image.elf  
avr-objcopy -Oihex -R.eeprom image.elf image.hex -v       

avr-size -A image.elf