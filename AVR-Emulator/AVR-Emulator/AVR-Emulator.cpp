// AVR-Emulator.cpp : Ce fichier contient la fonction 'main'. L'ex�cution du programme commence et se termine � cet endroit.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <cctype>
#include <list>
#include <SDL.h>


#define PIXEL_SIZE 5

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64


#define WIDTH SCREEN_WIDTH*PIXEL_SIZE
#define HEIGHT SCREEN_HEIGHT*PIXEL_SIZE



using namespace std;

struct Dico {
    uint8_t Reg;
    char Alias[256];
}test;


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


char * Program = NULL;
int ProgramSize = 0;

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

    return fsize;
}

void LoadProgram(const char* filename)
{
    long fsize = GetProgramSize(filename);
    printf("Program size: %d\n", fsize);
    ProgramSize = fsize+3;
    Program = (char *)malloc(ProgramSize);
    if (Program == NULL) {
        return;
    }
    FILE* f = fopen(filename, "rb");
    if (f == NULL)
    {
        printf("Error: Could not open file %s\n", filename);
        return;
    }

    fread(Program, 1, fsize, f);
    Program[fsize] = '\r';
    Program[fsize+1] = '\n';
    Program[fsize+2] = '\0';
    printf("Program = %s\n", Program);
    fclose(f);
}
/*
void GetLines()
{
    char* line = strtok((char*)Program, "\n");
    while (line != NULL)
    {
        printf("%s\n", line);
        line = strtok(NULL, "\n");
        lineCount++;
    }
}*/

void OutputPix(SDL_Renderer* renderer, SDL_Texture* buffer) {

    printf("outputing pixel, X = %d, Y = %d, color = 0x%x%x\n", registers[17], registers[18], registers[22], registers[21]);
    SDL_Rect rect;
    rect.x = PIXEL_SIZE*registers[17];
    rect.y = PIXEL_SIZE*registers[18];
    rect.w = PIXEL_SIZE;
    rect.h = PIXEL_SIZE;
    uint16_t color = registers[22] | registers[21]<<8;
    uint8_t R = color >> 11;
    uint8_t G = (color >> 5) & 0b111111;
    uint8_t B = color & 0b11111;
    R *= 8;
    G *= 4;
    B *= 8;
    SDL_SetRenderDrawColor(renderer, R, G, B, 255);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderPresent(renderer);
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

void Ins_INC(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    uint8_t result = rd_val + 1;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
}

void Ins_JMP(uint32_t k){
    pc = k-2;
}

void Ins_LAC(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    uint8_t result = rd_val & registers[30];
    registers[30] = result;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
}

void Ins_LAS(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    uint8_t result = rd_val | registers[30];
    registers[30] = result;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
}

void Ins_LAT(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    uint8_t result = rd_val ^ registers[30];
    registers[30] = result;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
}

void Ins_LD_X(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    registers[rd] = ram[registers[26] + registers[27] << 8];
}

void Ins_LD_X_INC(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    registers[rd] = ram[registers[26] + registers[27] << 8];
    registers[26] += 1;
}

void Ins_LD_X_DEC(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    registers[rd] = ram[registers[26] + registers[27] << 8];
    registers[26] -= 1;
}

void Ins_LD_Y(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    registers[rd] = ram[registers[28] + registers[29] << 8];
}

void Ins_LD_Y_INC(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    registers[rd] = ram[registers[28] + registers[29] << 8];
    registers[28] += 1;
}

void Ins_LD_Y_DEC(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    registers[rd] = ram[registers[28] + registers[29] << 8];
    registers[28] -= 1;
}

void Ins_LD_Z(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    registers[rd] = ram[registers[30] + registers[31] << 8];
}

void Ins_LD_Z_INC(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    registers[rd] = ram[registers[30] + registers[31] << 8];
    registers[30] += 1;
}

void Ins_LD_Z_DEC(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    registers[rd] = ram[registers[30] + registers[31] << 8];
    registers[30] -= 1;
}

void Ins_LDI(uint8_t rd, uint8_t k)
{
    registers[rd] = k;
}

void Ins_LPM(uint8_t rd, uint8_t rr)
{
    // TODO
}

void Ins_LSL(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    uint8_t result = rd_val << 1;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
    flag_V = flag_N ^ (result & 0x40);
    flag_C = rd_val & 0x80;
}

void Ins_LSR(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    uint8_t result = rd_val >> 1;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = false;
    flag_S = flag_N ^ flag_V;
    flag_V = flag_N ^ (result & 0x40);
    flag_C = rd_val & 0x01;
}

void Ins_MOV(uint8_t rd, uint8_t rr)
{
    uint8_t rr_val = registers[rr];
    registers[rd] = rr_val;
}

void Ins_MOVW(uint8_t rd, uint8_t rr)
{
    uint16_t rr_val = registers[rr] + registers[rr + 1] << 8;
    registers[rd] = rr_val & 0xFF;
    registers[rd + 1] = (rr_val >> 8) & 0xFF;
}

void Ins_MUL(uint8_t rd, uint8_t rr)
{
    uint8_t rd_val = registers[rd];
    uint8_t rr_val = registers[rr];
    uint16_t result = rd_val * rr_val;
    registers[0] = (result >> 8) & 0xFF;
    registers[1] = result & 0xFF;
}

void Ins_MULS(uint8_t rd, uint8_t rr)
{
    int8_t rd_val = registers[rd];
    int8_t rr_val = registers[rr];
    int16_t result = rd_val * rr_val;
    registers[0] = (result >> 8) & 0xFF;
    registers[1] = result & 0xFF;
}

void Ins_MULSU(uint8_t rd, uint8_t rr)
{
    int8_t rd_val = registers[rd];
    uint8_t rr_val = registers[rr];
    int16_t result = rd_val * rr_val;
    registers[0] = (result >> 8) & 0xFF;
    registers[1] = result & 0xFF;
}

void Ins_NEG(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    uint8_t result = -rd_val;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
    flag_V = (rd_val == 0x80);
    flag_C = (rd_val != 0);
}

void Ins_NOP()
{
    // Do nothing
}

void Ins_OR(uint8_t rd, uint8_t rr)
{
    uint8_t rd_val = registers[rd];
    uint8_t rr_val = registers[rr];
    uint8_t result = rd_val | rr_val;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
    flag_V = false;
    flag_C = false;
}

void Ins_ORI(uint8_t rd, uint8_t k)
{
    uint8_t rd_val = registers[rd];
    uint8_t result = rd_val | k;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
    flag_V = false;
    flag_C = false;
}

void Ins_OUT(uint8_t A, uint8_t rr)
{
    // TODO: Implement
}

void Ins_POP(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    registers[rd] = *stackPointer;
    stackPointer++;
}

void Ins_PUSH(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    stackPointer--;
    *stackPointer = rd_val;
}

void Ins_RCALL(uint8_t k)
{
    stackPointer--;
    *stackPointer = (pc >> 8) & 0xFF;
    stackPointer--;
    *stackPointer = pc & 0xFF;
    pc += k;
}

void Ins_RET()
{
    pc = *stackPointer;
    stackPointer++;
    pc += *stackPointer << 8;
    stackPointer++;
}

void Ins_RETI()
{
    pc = *stackPointer;
    stackPointer++;
    pc += *stackPointer << 8;
    stackPointer++;
    flag_I = true;
}

void Ins_RJMP(uint8_t k)
{
    pc += k;
}

void Ins_ROL(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    uint8_t result = (rd_val << 1) | flag_C;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = false;
    flag_S = flag_N ^ flag_V;
    flag_V = flag_N ^ (result & 0x40);
    flag_C = rd_val & 0x80;
}

void Ins_ROR(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    uint8_t result = (rd_val >> 1) | (flag_C << 7);
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = false;
    flag_S = flag_N ^ flag_V;
    flag_V = flag_N ^ (result & 0x40);
    flag_C = rd_val & 0x01;
}

void Ins_SBC(uint8_t rd, uint8_t rr)
{
    uint8_t rd_val = registers[rd];
    uint8_t rr_val = registers[rr];
    uint8_t result = rd_val - rr_val - flag_C;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
    flag_V = (rd_val & 0x80) && !(rr_val & 0x80) && !(result & 0x80);
    flag_C = (rd_val & 0x80) && !(rr_val & 0x80) || ((rd_val & 0x80) || !(rr_val & 0x80)) && !(result & 0x80);
}

void Ins_SBCI(uint8_t rd, uint8_t k)
{
    uint8_t rd_val = registers[rd];
    uint8_t result = rd_val - k - flag_C;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
    flag_V = (rd_val & 0x80) && !(k & 0x80) && !(result & 0x80);
    flag_C = (rd_val & 0x80) && !(k & 0x80) || ((rd_val & 0x80) || !(k & 0x80)) && !(result & 0x80);
}

void Ins_SBI(uint8_t A, uint8_t b)
{
    uint8_t A_val = registers[A];
    uint8_t result = A_val | (1 << b);
    registers[A] = result;
}

void Ins_SBIC(uint8_t A, uint8_t b)
{
    uint8_t A_val = registers[A];
    if (!(A_val & (1 << b)))
    {
        pc += 2;
    }
}

void Ins_SBIS(uint8_t A, uint8_t b)
{
    uint8_t A_val = registers[A];
    if (A_val & (1 << b))
    {
        pc += 2;
    }
}

void Ins_SBIW(uint8_t A, uint8_t b)
{
    uint16_t A_val = registers[A];
    uint16_t result = A_val - b;
    registers[A] = result & 0xFF;
    registers[A + 1] = (result >> 8) & 0xFF;
    flag_Z = (result == 0);
    flag_N = (result & 0x8000);
    flag_S = flag_N ^ flag_V;
    flag_V = (A_val & 0x8000) && !(b & 0x8000) && !(result & 0x8000);
    flag_C = (A_val & 0x8000) && !(b & 0x8000) || ((A_val & 0x8000) || !(b & 0x8000)) && !(result & 0x8000);
}

void Ins_SBR(uint8_t rd, uint8_t k)
{
    uint8_t rd_val = registers[rd];
    uint8_t result = rd_val | k;
    registers[rd] = result;
    flag_S = flag_N ^ flag_V;
    flag_V = false;
    flag_N = (result & 0x80);
    flag_Z = (result == 0);
}

void Ins_SBRC(uint8_t rr, uint8_t b)
{
    uint8_t rr_val = registers[rr];
    if (!(rr_val & (1 << b)))
    {
        pc += 2;
    }
}

void Ins_SBRS(uint8_t rr, uint8_t b)
{
    uint8_t rr_val = registers[rr];
    if (rr_val & (1 << b))
    {
        pc += 2;
    }
}

void Ins_SEC()
{
    flag_C = true;
}

void Ins_SEH()
{
    flag_H = true;
}

void Ins_SEI()
{
    flag_I = true;
}

void Ins_SEN()
{
    flag_N = true;
}

void Ins_SER(uint8_t rd)
{
    registers[rd] = 0xFF;
    flag_Z = false;
    flag_N = true;
    flag_S = true;
    flag_V = false;
}

void Ins_SES()
{
    flag_S = true;
}

void Ins_SET()
{
    flag_T = true;
}

void Ins_SEV()
{
    flag_V = true;
}

void Ins_SEZ()
{
    flag_Z = true;
}

void Ins_SLEEP()
{
    // Nothing
}

void Ins_ST_X(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    ram[registers[26] + registers[27] << 8] = rd_val;
}

void Ins_ST_X_INC(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    ram[registers[26] + registers[27] << 8] = rd_val;
    registers[26] += 1;
}

void Ins_ST_X_DEC(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    ram[registers[26] + registers[27] << 8] = rd_val;
    registers[26] -= 1;
}

void Ins_ST_Y(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    ram[registers[28] + registers[29] << 8] = rd_val;
}

void Ins_ST_Y_INC(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    ram[registers[28] + registers[29] << 8] = rd_val;
    registers[28] += 1;
}

void Ins_ST_Y_DEC(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    ram[registers[28] + registers[29] << 8] = rd_val;
    registers[28] -= 1;
}

void Ins_ST_Z(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    ram[registers[30] + registers[31] << 8] = rd_val;
}

void Ins_ST_Z_INC(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    ram[registers[30] + registers[31] << 8] = rd_val;
    registers[30] += 1;
}

void Ins_ST_Z_DEC(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    ram[registers[30] + registers[31] << 8] = rd_val;
    registers[30] -= 1;
}

void Ins_STS(uint8_t rd)
{
    // TODO
}

void Ins_SUB(uint8_t rd, uint8_t rr)
{
    uint8_t rd_val = registers[rd];
    uint8_t rr_val = registers[rr];
    uint8_t result = rd_val - rr_val;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
}

void Ins_SUBI(uint8_t rd, uint8_t k)
{
    uint8_t rd_val = registers[rd];
    uint8_t result = rd_val - k;
    registers[rd] = result;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
}

void Ins_SWAP(uint8_t rd)
{
    uint8_t rd_val = registers[rd];
    uint8_t result = (rd_val << 4) | (rd_val >> 4);
    registers[rd] = result;
}

void Ins_TST(uint8_t rd, uint8_t rr)
{
    uint8_t rd_val = registers[rd];
    uint8_t rr_val = registers[rr];
    uint8_t result = rd_val & rr_val;
    flag_Z = (result == 0);
    flag_N = (result & 0x80);
    flag_S = flag_N ^ flag_V;
}


void Ins_WDR()
{
    // TODO
}

void Ins_XCH(uint8_t rd, uint8_t rr)
{
    uint8_t rd_val = registers[rd];
    uint8_t rr_val = registers[rr];
    registers[rd] = rr_val;
    registers[rr] = rd_val;
}

uint8_t Decode_Regiser(char * Reg){
    if (Reg == NULL) return 0;
    char * NumReg = &Reg[1];
    uint8_t RegNum = 0;
    if (NumReg != NULL) {
        RegNum = atoi(NumReg);
    }
    return RegNum;
}

void Format_Program(){
    // remove comments
    for (int i = 0; i < ProgramSize; i++)
    {
        if(Program[i] == ';'){
            while(Program[i] != '\n'){
                Program[i] = ' ';
                i++;
            }
        }
    }
    // remove empty lines
    for (int j = 0; j < 512; j++)
    {   
        for (int i = 0; i < ProgramSize; i++)
        {
            if(Program[i] == '\n' && Program[i+2] == '\n'){
                Program[i-1] = ' ';
                Program[i] = ' ';
            }
        }
    }
    // remove tabs
    for (int i = 0; i < ProgramSize; i++)
    {
        if(Program[i] == '\t'){
            Program[i] = ' ';
        }
    }


}

void SetLines(){
    
    int line = 0;
    for (int i = 0; i < ProgramSize; i++){
        if (Program[i] == '\n') line++;
    }

    
    //printf("Program = %s\n", Program);
    Lines = (char ** ) malloc(sizeof(char*) * line);
    //printf("Line count = %d\n", line);
    int j = 0;
    for (int i = 0; i < line; i++){
        int k = 0;
        char* CurrLine = (char *) malloc(sizeof(char) * 512);
        while(Program[j] != '\n' && Program[j] != '\0'){
                CurrLine[k] = Program[j];
                //printf("CurrLine[%d] = %c\n", j, CurrLine[k]);
                k++;
                j++;
        }
        CurrLine[k] = '\0';
        Lines[i] = CurrLine;
        //replace the newline with the null character
        j++;
        
        //printf("Line decoded = %s\n", Lines[i]);
    }
    lineCount = line;
    for (int i = 0; i < line; i++)
    {
       // printf("Line decoded = %s\n", Lines[i]);
    }
    
    
}

void Decode_Ins(int line, SDL_Renderer* renderer, SDL_Texture* buffer){


    
    char* lineStr = Lines[line];
    printf("Decoding line %d\n", line);
    printf("line = %s\n", lineStr);
    

    char* opcode = strtok(lineStr, " ");
    
    char* arg1 = strtok(NULL, " ");
    char* arg2 = strtok(NULL, " ");
    char* arg3 = strtok(NULL, " ");
    printf("Line %d = op :%s, arg1 = %s\n", line, opcode, arg1);
    //cout << line << " " << opcode << " " << arg1 << " " << arg2 << " " << arg3 << endl;
    for (int i = 0; i < sizeof(opcode); i++)
    {
        opcode[i] = tolower(opcode[i]);
    }
    if (arg1 != NULL) {
        for (int i = 0; i < sizeof(arg1); i++)
        {
            arg1[i] = tolower(arg1[i]);
        }
    }
    if (arg2 != NULL) {
        for (int i = 0; i < sizeof(arg2); i++)
        {
            arg2[i] = tolower(arg2[i]);
        }
    }
    if (arg3 != NULL) {
        for (int i = 0; i < sizeof(arg3); i++)
        {
            arg3[i] = tolower(arg3[i]);
        }
    }
    printf("Line %d = op :%s, arg1 = %s\n", line, opcode, arg1);
    for (int i = 0; opcode[i] != '\0'; i++)
    {
        printf("0x%x, ", opcode[i]);
    }   
    printf("\n");
    if (arg1 != NULL) {
        for (int i = 0; arg1[i] != '\0'; i++)
        {
            printf("0x%x, ", arg1[i]);
        }
    }
    printf("\n");
    if (strcmp(opcode, "rjmp") == 0) OutputPix(renderer, buffer);
    if (strcmp(opcode, "ldi") == 0) Ins_LDI((arg1 != NULL) ? Decode_Regiser(arg1) : 0, (arg2 != NULL) ? atoi(arg2) : 0);
    if (strcmp(opcode, "inc") == 0) Ins_INC((arg1 != NULL) ? Decode_Regiser(arg1) : 0);
    if (strcmp(opcode, "jmp") == 0) Ins_JMP((arg1 != NULL) ? atoi(arg1) : 0);

}


int Start(SDL_Renderer* renderer, SDL_Texture* buffer) {

    char filename[] = "Main.asm";
    LoadProgram(filename);
    Format_Program();
    //printf("Program = %s\n", Program);
    SetLines();
    printf("Line Count: %d\n", lineCount);
    pc = 0;
    while (pc < lineCount)
    {
        printf("PC: %d\n", pc);
        printf("Program = %s\n", Program);
        SetLines();
        Decode_Ins(pc, renderer, buffer);
        pc++;
    }
    return 0;
}


int main(int argc, char** args)
{
    SDL_Renderer* renderer;
    SDL_Window* window;



    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_Texture* buffer = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGB565,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH,
        HEIGHT);
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);

    Start(renderer, buffer);
    SDL_Event event;
    while (1) {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
