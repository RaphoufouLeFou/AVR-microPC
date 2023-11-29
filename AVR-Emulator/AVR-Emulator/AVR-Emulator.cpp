// AVR-Emulator.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// X = R27:R26, Y = R29:R28, Z = R31:R30
uint8_t registers[32];
uint8_t ram[1024];
uint16_t pc;

uint8_t * stackPointer;

bool flag_C;
bool flag_Z;
bool flag_N;
bool flag_V;
bool flag_S;
bool flag_H;
bool flag_T;
bool flag_I;


uint8_t * Program = NULL;

char** Lines = NULL;

int lineCount = 0;

long GetProgramSize(const char* filename)
{
    FILE* f = fopen(filename, "rb");
    if (f == NULL)
    {
        printf("Error: Could not open file %s\n", filename);
        return 0;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    fclose(f);

    return fsize -1;
}

void LoadProgram(const char* filename)
{
    long fsize = GetProgramSize(filename);
    printf("Program size: %d\n", fsize);
    Program = (uint8_t*)malloc(fsize + 1);

    FILE* f = fopen(filename, "rb");
    if (f == NULL)
    {
        printf("Error: Could not open file %s\n", filename);
        return;
    }

    fread(Program, fsize, 1, f);
    fclose(f);
}

void GetLines()
{
    char* line = strtok((char*)Program, "\n");
    while (line != NULL)
    {
        printf("%s\n", line);
        line = strtok(NULL, "\n");
        lineCount++;
    }
}

void Ins_ADC(uint8_t rd, uint8_t rr)
{
    uint8_t rd_val = registers[rd];
    uint8_t rr_val = registers[rr];
    uint8_t result = rd_val + rr_val + flag_C;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
    flag_V = (rd_val & 0x80) && (rr_val & 0x80) && !(result & 0x80);
    flag_C = (rd_val & 0x80) && (rr_val & 0x80) || ((rd_val & 0x80) || (rr_val & 0x80)) && !(result & 0x80);
}

void Ins_ADD(uint8_t rd, uint8_t rr)
{
    uint8_t rd_val = registers[rd];
    uint8_t rr_val = registers[rr];
    uint8_t result = rd_val + rr_val;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
    flag_V = (rd_val & 0x80) && (rr_val & 0x80) && !(result & 0x80);
    flag_C = (rd_val & 0x80) && (rr_val & 0x80) || ((rd_val & 0x80) || (rr_val & 0x80)) && !(result & 0x80);
}

void Ins_ADIW(uint8_t rd, uint8_t rr)
{
    uint16_t rd_val = registers[rd];
    uint16_t rr_val = registers[rr];
    uint16_t result = rd_val + rr_val;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = (result & 0x8000);
    flag_S = flag_N ^ flag_V;
    flag_V = (rd_val & 0x8000) && (rr_val & 0x8000) && !(result & 0x8000);
    flag_C = (rd_val & 0x8000) && (rr_val & 0x8000) || ((rd_val & 0x8000) || (rr_val & 0x8000)) && !(result & 0x8000);
}

void Ins_AND(uint8_t rd, uint8_t rr)
{
    uint8_t rd_val = registers[rd];
    uint8_t rr_val = registers[rr];
    uint8_t result = rd_val & rr_val;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
    flag_V = false;
    flag_C = false;
}

void Ins_ANDI(uint8_t rd, uint8_t i)
{
    uint8_t rd_val = registers[rd];
    uint8_t result = rd_val & i;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
    flag_V = false;
    flag_C = false;
}

void Ins_ASR(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    uint8_t result = rd_val >> 1;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
    flag_V = flag_N ^ (result & 0x40);
    flag_C = rd_val & 0x01;
}

void Ins_BCLR(uint8_t s)
{
    switch (s)
    {
    case 0:
        flag_C = false;
        break;
    case 1:
        flag_Z = false;
        break;
    case 2:
        flag_N = false;
        break;
    case 3:
        flag_V = false;
        break;
    case 4:
        flag_S = false;
        break;
    case 5:
        flag_H = false;
        break;
    case 6:
        flag_T = false;
        break;
    case 7:
        flag_I = false;
        break;
    }
}

void Ins_BLD(uint8_t rd, uint8_t b)
{
    uint8_t rd_val = registers[rd];
    uint8_t result = rd_val | (flag_T << b);
    registers[rd] = result;
}

void Ins_BRBC(uint8_t s, uint8_t k)
{
    if (!flag_T)
    {
        pc += k;
    }
}

void Ins_BRBS(uint8_t s, uint8_t k)
{
    if (flag_T)
    {
        pc += k;
    }
}

void Ins_BRCC(uint8_t k)
{
    if (!flag_C)
    {
        pc += k;
    }
}

void Ins_BRCS(uint8_t k)
{
    if (flag_C)
    {
        pc += k;
    }
}

void Ins_BREAK()
{
    printf("BREAK\n");
}

void Ins_BREQ(uint8_t k)
{
    if (flag_Z)
    {
        pc += k;
    }
}

void Ins_BRGE(uint8_t k)
{
    if (flag_S == flag_V)
    {
        pc += k;
    }
}

void Ins_BRHC(uint8_t k)
{
    if (!flag_H)
    {
        pc += k;
    }
}

void Ins_BRHS(uint8_t k)
{
    if (flag_H)
    {
        pc += k;
    }
}

void Ins_BRID(uint8_t k)
{
    if (!flag_I)
    {
        pc += k;
    }
}

void Ins_BRIE(uint8_t k)
{
    if (flag_I)
    {
        pc += k;
    }
}

void Ins_BRLO(uint8_t k)
{
    if (!flag_C)
    {
        pc += k;
    }
}

void Ins_BRLT(uint8_t k)
{
    if (flag_S != flag_V)
    {
        pc += k;
    }
}

void Ins_BRMI(uint8_t k)
{
    if (flag_N)
    {
        pc += k;
    }
}

void Ins_BRNE(uint8_t k)
{
    if (!flag_Z)
    {
        pc += k;
    }
}

void Ins_BRPL(uint8_t k)
{
    if (!flag_N)
    {
        pc += k;
    }
}

void Ins_BRSH(uint8_t k)
{
    if (!flag_C)
    {
        pc += k;
    }
}

void Ins_BRTC(uint8_t k)
{
    if (!flag_T)
    {
        pc += k;
    }
}

void Ins_BRTS(uint8_t k)
{
    if (flag_T)
    {
        pc += k;
    }
}

void Ins_BRVC(uint8_t k)
{
    if (!flag_V)
    {
        pc += k;
    }
}

void Ins_BRVS(uint8_t k)
{
    if (flag_V)
    {
        pc += k;
    }
}

void Ins_BSET(uint8_t s)
{
    switch (s)
    {
    case 0:
        flag_C = true;
        break;
    case 1:
        flag_Z = true;
        break;
    case 2:
        flag_N = true;
        break;
    case 3:
        flag_V = true;
        break;
    case 4:
        flag_S = true;
        break;
    case 5:
        flag_H = true;
        break;
    case 6:
        flag_T = true;
        break;
    case 7:
        flag_I = true;
        break;
    }
}

void Ins_BST(uint8_t rd, uint8_t b)
{
    uint8_t rd_val = registers[rd];
    flag_T = (rd_val >> b) & 0x01;
}

void Ins_CALL(uint16_t k)
{
    stackPointer--;
    *stackPointer = (pc >> 8) & 0xFF;
    stackPointer--;
    *stackPointer = pc & 0xFF;
    pc = k;
}

void Ins_CBI(uint8_t A, uint8_t b)
{
    uint8_t A_val = registers[A];
    uint8_t result = A_val & ~(1 << b);
    registers[A] = result;
}

void Ins_CBR(uint8_t rd, uint8_t k)
{
    uint8_t rd_val = registers[rd];
    uint8_t result = rd_val & ~k;
    registers[rd] = result;
    flag_S = flag_N ^ flag_V;
    flag_V = false;
    flag_N = (result & 0x80);
    flag_Z = (result == 0);
}

void Ins_CLC()
{
    flag_C = false;
}

void Ins_CLH()
{
    flag_H = false;
}

void Ins_CLI()
{
    flag_I = false;
}

void Ins_CLN()
{
    flag_N = false;
}

void Ins_CLR(uint8_t rd)
{
    registers[rd] = 0;
    flag_Z = true;
    flag_N = false;
    flag_S = false;
    flag_V = false;
}

void Ins_CLS()
{
    flag_S = false;
}

void Ins_CLT()
{
    flag_T = false;
}

void Ins_CLV()
{
    flag_V = false;
}

void Ins_CLZ()
{
    flag_Z = false;
}

void Ins_COM(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    uint8_t result = ~rd_val;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
    flag_V = false;
    flag_C = true;
}

void Ins_CP(uint8_t rd, uint8_t rr)
{
    uint8_t rd_val = registers[rd];
    uint8_t rr_val = registers[rr];
    uint8_t result = rd_val - rr_val;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
    flag_V = (rd_val & 0x80) && !(rr_val & 0x80) && !(result & 0x80);
    flag_C = (rd_val & 0x80) && !(rr_val & 0x80) || ((rd_val & 0x80) || !(rr_val & 0x80)) && !(result & 0x80);
}

void Ins_CPC(uint8_t rd, uint8_t rr)
{
    uint8_t rd_val = registers[rd];
    uint8_t rr_val = registers[rr];
    uint8_t result = rd_val - rr_val - flag_C;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
    flag_V = (rd_val & 0x80) && !(rr_val & 0x80) && !(result & 0x80);
    flag_C = (rd_val & 0x80) && !(rr_val & 0x80) || ((rd_val & 0x80) || !(rr_val & 0x80)) && !(result & 0x80);
}

void Ins_CPI(uint8_t rd, uint8_t k)
{
    uint8_t rd_val = registers[rd];
    uint8_t result = rd_val - k;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
    flag_V = (rd_val & 0x80) && !(k & 0x80) && !(result & 0x80);
    flag_C = (rd_val & 0x80) && !(k & 0x80) || ((rd_val & 0x80) || !(k & 0x80)) && !(result & 0x80);
}

void Ins_CPSE(uint8_t rd, uint8_t rr)
{
    uint8_t rd_val = registers[rd];
    uint8_t rr_val = registers[rr];
    if (rd_val == rr_val)
    {
        pc += 2;
    }
}

void Ins_DEC(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    uint8_t result = rd_val - 1;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
}

void Ins_EICALL(){
    stackPointer--;
    *stackPointer = (pc >> 8) & 0xFF;
    stackPointer--;
    *stackPointer = pc & 0xFF;
    pc = registers[30];
}

void Ins_EIJMP(){
    pc = registers[30];
}

void Ins_ELPM(uint8_t rd, uint8_t rr)
{
    uint8_t rr_val = registers[rr];
    registers[rd] = ram[rr_val];
}

void Ins_EOR(uint8_t rd, uint8_t rr)
{
    uint8_t rd_val = registers[rd];
    uint8_t rr_val = registers[rr];
    uint8_t result = rd_val ^ rr_val;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
    flag_V = false;
    flag_C = false;
}

void Ins_FMUL(uint8_t rd, uint8_t rr)
{
    uint8_t rd_val = registers[rd];
    uint8_t rr_val = registers[rr];
    uint16_t result = rd_val * rr_val;
    registers[0] = (result >> 8) & 0xFF;
    registers[1] = result & 0xFF;
}

void Ins_FMULS(uint8_t rd, uint8_t rr)
{
    int8_t rd_val = registers[rd];
    int8_t rr_val = registers[rr];
    int16_t result = rd_val * rr_val;
    registers[0] = (result >> 8) & 0xFF;
    registers[1] = result & 0xFF;
}

void Ins_FMULSU(uint8_t rd, uint8_t rr)
{
    int8_t rd_val = registers[rd];
    uint8_t rr_val = registers[rr];
    int16_t result = rd_val * rr_val;
    registers[0] = (result >> 8) & 0xFF;
    registers[1] = result & 0xFF;
}

void Ins_ICALL()
{
    stackPointer--;
    *stackPointer = (pc >> 8) & 0xFF;
    stackPointer--;
    *stackPointer = pc & 0xFF;
    pc = registers[30] + registers[31] << 8;
}

void Ins_IJMP()
{
    pc = registers[30] + registers[31] << 8;
}





int main()
{
    char filename[] = "Main.asm";
    LoadProgram(filename);
    GetLines();
    printf("Line Count: %d\n", lineCount);
    return 0;
}
