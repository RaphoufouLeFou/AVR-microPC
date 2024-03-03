avr-g++ -c -O3 -w -mmcu=ATmega16 -DF_CPU=16000000L arduinoCppCode.cpp -o outC.o
avr-g++ -c -O3 -w -mmcu=ATmega16 -DF_CPU=16000000L isrs.S -o outS.o

avr-gcc -O3 -Wl,--gc-sections -mmcu=atmega16 outC.o outS.o -o image.elf  
avr-objcopy -Oihex -R.eeprom image.elf image.hex -v

"C:\Users\rapha\AppData\Local\Arduino15\packages\MightyCore\tools\avrdude\7.1-arduino.1/bin/avrdude" "-CC:\Users\rapha\AppData\Local\Arduino15\packages\MightyCore\hardware\avr\2.2.1/avrdude.conf" -v -V -patmega16 -carduino -PCOM1 -b115200 -D "-Uflash:w:D:\DOCUMENTS\GitHub\AVR-dev\arduinoCppCode\image.hex:i"

