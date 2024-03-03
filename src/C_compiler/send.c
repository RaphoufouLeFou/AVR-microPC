#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void ThrowException(char * message){
    printf("Error : %s\n", message);
    exit(1);
}

int main (int argc, char **argv) {

    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "help") == 0){
        printf("Usage: send [Optimization] [Cfilename] [Sfilename] [send]\n");
        printf("Optimization : -O0, -O1, -O2, -O3, -Os\n");
        printf("Cfilename : C++ (.cpp) file name\n");
        printf("Sfilename : ASM (.S) file name\n");
        printf("send : -r (run) or -s (send), -c (compile)\n");
        return 0;
    }

    if(argc != 5) ThrowException("Invalid number of arguments");

    char * Optimization = argv[1];
    char * Cfilename = argv[2];
    char * Sfilename = argv[3];
    char * send = argv[4];

    int CfileNameSize = strlen(Cfilename) + 63;
    int SfileNameSize = strlen(Cfilename) + 63;
    
    char* Command1 = 0;
    char* Command2 = 0;
    char* Command3 = 0;
    char* Command4 = "avr-objcopy -Oihex -R.eeprom image.elf image.hex -v";
    char* Command5;

    Command1 = (char*)malloc(CfileNameSize * sizeof(char));
    memcpy(Command1, "avr-g++ -c ", 12 * sizeof(char));
    strcat(Command1, Optimization);
    strcat(Command1, " -w -mmcu=ATmega16 -DF_CPU=16000000L ");
    strcat(Command1, Cfilename);
    strcat(Command1, " -o outC.o");
    Command2 = (char*)malloc(SfileNameSize * sizeof(char));
    memcpy(Command2, "avr-g++ -c ", 12 * sizeof(char));
    strcat(Command2, Optimization);
    strcat(Command2, " -w -mmcu=ATmega16 -DF_CPU=16000000L ");
    strcat(Command2, Sfilename);
    strcat(Command2, " -o outS.o");
    Command3 = (char*)malloc(72 * sizeof(char));
    memcpy(Command3, "avr-gcc ", 9 * sizeof(char));
    strcat(Command3, Optimization);
    strcat(Command3, " -Wl,--gc-sections -mmcu=atmega16 outC.o outS.o -o image.elf");

    if(strcmp(send, "-r") == 0 || strcmp(send, "-s") == 0)
        Command5 = "Run.bat";
    else if (strcmp(send, "-c") == 0)
        Command5 = "avr-size -A image.elf";
    else(ThrowException("Invalid Send Flag"));

    system(Command1);
    system(Command2);
    system(Command3);
    system(Command4);
    system(Command5);
    return 0;
}