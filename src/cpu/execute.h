#include "instructions.h"

/*
read from memory
*/
void execute_instruction(struct CPU *cpu) {
    //printf("%02X %02X %02X %02X - %02X\n", cpu->reg[0],cpu->reg[1],cpu->reg[2],cpu->reg[3],cpu->PC);
    switch (opcode_mask(cpu->MEM[cpu->PC])) {
        case 0b0000:
            NUL(cpu);
            break;
        case 0b0001:
            MW(cpu);
            break;
        case 0b0010:
            LW(cpu);
            break;
        case 0b0011:
            SW(cpu);
            break;
        case 0b0100:
            LPC(cpu);
            break;
        case 0b0101:
            IN(cpu);
            break;
        case 0b0110:
            OUT(cpu);
            break;
        case 0b0111:
            JNZ(cpu);
            break;
        case 0b1000:
            CMP(cpu);
            break;
        case 0b1001:
            ADC(cpu);
            break;
        case 0b1010:
            SBB(cpu);
            break;
        case 0b1011:
            AND(cpu);
            break;
        case 0b1100:
            XOR(cpu);
            break;
        case 0b1101:
            PUSH(cpu);
            break;
        case 0b1110:
            POP(cpu);
            break;
        case 0b1111:
            HLT(cpu);
            break;
        default:
            break; 
    }
}

