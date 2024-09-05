#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "../shared/constants.h"

    /*
    NUL
    1 byte

    increment cpu->PC, nothing else is done
    */
void NUL(struct CPU *cpu) {
    if (modifier_mask(cpu->MEM[cpu->PC])) { // INT
        uint8_t int_code = reg_mask(cpu->MEM[cpu->PC]);
        uint8_t block_A = cpu->reg[0];
        uint8_t byte_D = cpu->reg[3];
        uint8_t byte_Z = cpu->reg[4];
        uint8_t flags = cpu->reg[5];
        switch (int_code) {
            case 0b000: // read file system data
                cpu->reg[4] = fs_read_data(&(cpu->FS), block_A, byte_D);
                cpu->reg[5] |= cpu->FS.flags;
                break;
            case 0b001: // write to file system
                fs_write_data(&(cpu->FS), block_A, byte_D, byte_Z, flags);
                break;
            case 0b010: // create block
                cpu->reg[4] = fs_create_file(&(cpu->FS));
                break;
            default:
                break;
        }
    }
    cpu->PC++;
    
}

    /*
    MW ra $imm8
    2 bytes
    --

    MW ra rb
    2 bytes
    --

    sets ra to arg2
    */
void MW(struct CPU *cpu) {
    if (modifier_mask(cpu->MEM[cpu->PC])) {
        uint8_t reg1 = reg_mask(cpu->MEM[cpu->PC]);
        cpu->PC++;
        uint8_t reg2 = reg_mask(cpu->MEM[cpu->PC]);
        cpu->reg[reg1] = cpu->reg[reg2];
        cpu->PC++;
    } else {
        uint8_t reg1 = reg_mask(cpu->MEM[cpu->PC]);
        cpu->PC++;
        uint8_t val2 = cpu->MEM[cpu->PC];
        cpu->reg[reg1] = val2;
        cpu->PC++;
    }
}

    /*
    LW ra $imm16
    3 bytes
    moves [$] to ra

    LW ra rb
    2 bytes
    moves [rb, rb+1] to ra
    */
void LW(struct CPU *cpu) {

    uint8_t reg1 = reg_mask(cpu->MEM[cpu->PC]);
    if (modifier_mask(cpu->MEM[cpu->PC])) {
        cpu->PC++;
        uint8_t reg2 = reg_mask(cpu->MEM[cpu->PC]);
        uint16_t memory_address = ((unsigned short)cpu->reg[reg2] << 8) | (unsigned short)cpu->reg[(reg2 + 1) % 8];
        cpu->reg[reg1] = cpu->MEM[memory_address];
        cpu->PC++;
    } else {
        cpu->PC++;
        uint16_t imm1 = (cpu->MEM[cpu->PC]);
        uint16_t imm2 = (cpu->MEM[cpu->PC+1]);
        uint16_t memory_address = (imm1 << 8) | (imm2);
        cpu->reg[reg1] = cpu->MEM[memory_address];
        cpu->PC += 2;
    }
}

    /*
    SW $imm16 rb
    3 bytes
    stores rb at [$]

    SW ra rb
    2 bytes
    stores rb at [ra ra+1] 
    */
void SW(struct CPU *cpu) {

    uint8_t reg1 = reg_mask(cpu->MEM[cpu->PC]);
    if (modifier_mask(cpu->MEM[cpu->PC])) {
        cpu->PC++;
        uint8_t reg2 = reg_mask(cpu->MEM[cpu->PC]);
        uint16_t address = (cpu->reg[reg1] << 8) | cpu->reg[(reg1+1) % 8];
        cpu->MEM[address] = cpu->reg[reg2];
        cpu->PC++;
    } else {
        cpu->PC++;
        uint16_t address = (cpu->MEM[cpu->PC] << 8) | cpu->MEM[cpu->PC + 1];
        cpu->MEM[address] = cpu->reg[reg1];
        cpu->PC += 2;
    }
}

    /*
    LPC ra
    1 byte

    loads upper byte of cpu->PC to ra, lower byte to ra+1

    or clear register if flag set
    */
void LPC(struct CPU *cpu) {

    uint8_t reg1 = reg_mask(cpu->MEM[cpu->PC]);
    if (modifier_mask(cpu->MEM[cpu->PC])) {
        cpu->reg[reg1] = 0;
        cpu->PC++;
    } else {
        cpu->reg[reg1] = ((cpu->PC + 1) & 0xFF00) >> 8;   
        cpu->reg[(reg1 + 1) % 8] = ((cpu->PC + 1) & 0x00FF); 
        cpu->PC++;
    }
    
}

    /*
    IN ra $imm8
    2 bytes
    recieve byte from cpu->DEVICE[$] and put in ra

    IN ra rb
    2 bytes
    recieve byte from cpu->DEVICE[rb] and put in ra
    */
void IN(struct CPU *cpu) {
    uint8_t reg1 = reg_mask(cpu->MEM[cpu->PC]);
    uint8_t val2;
    if (modifier_mask(cpu->MEM[cpu->PC])) {
        cpu->PC++;
        val2 = cpu->reg[reg_mask(cpu->MEM[cpu->PC])];
    } else {
        cpu->PC++;
        val2 = cpu->MEM[cpu->PC];
    }
    cpu->reg[reg1] = cpu->DEVICE[val2];
    cpu->PC++;
}

    /*
    OUT $imm8 ra
    2 bytes
    send byte ra to cpu->DEVICE[$]

    OUT ra rb
    2 bytes
    send byte rb to cpu->DEVICE[ra]
    */

void OUT(struct CPU *cpu) {
    uint8_t dev;
    uint8_t b;
    if (modifier_mask(cpu->MEM[cpu->PC])) {
        dev = reg_mask(cpu->MEM[cpu->PC]);
        cpu->PC++;
        b = cpu->reg[reg_mask(cpu->MEM[cpu->PC])];
    } else {
        // important to note this is reversed order from if modifier bit is set,
        // as we always pack the first register with the instruction
        b = reg_mask(cpu->MEM[cpu->PC]); 
        cpu->PC++;
        dev = cpu->MEM[cpu->PC];
    }
    cpu->DEVICE[dev] = b;
    cpu->PC++;
}

    /*
    JNZ $imm3
    1 byte
    jump to [r6 r7] if $ != 0

    JNZ ra
    1 byte
    jump to [r6 r7] if ra != 0
    */
void JNZ(struct CPU *cpu) {
    uint16_t address = (cpu->reg[6] << 8) | cpu->reg[7]; // HL registers
    if (modifier_mask(cpu->MEM[cpu->PC])) {
        uint8_t reg1 = reg_mask(cpu->MEM[cpu->PC]);
        if (cpu->reg[reg1] != 0) {
            cpu->PC = address;
        } else {
            cpu->PC++;
        }
    } else {
        if (reg_mask(cpu->MEM[cpu->PC]) != 0) {
            cpu->PC = address;
        } else {
            cpu->PC++;
        }
    }
}

    /*
    CMP ra rb
    2 bytes
    --

    CMP ra $imm8
    2 bytes
    --

    modifies r5 (flag register) as follows:
        sets lt bit if arg1 < arg2, otherwise sets 0
        sets eq bit if eq, otherwise sets 0
        sets zr bit if arg1 == 0 || arg2 == 0
    
    */
void CMP(struct CPU *cpu) {
    uint8_t reg1 = reg_mask(cpu->MEM[cpu->PC]);
    uint8_t val2;
    uint8_t new_val = cpu->reg[5];
    if (modifier_mask(cpu->MEM[cpu->PC])) {
        cpu->PC++;
        val2 = cpu->reg[reg_mask(cpu->MEM[cpu->PC])];
    } else {
        cpu->PC++;
        val2 = cpu->MEM[cpu->PC];
    }

    if (cpu->reg[reg1] == 0 || val2 == 0) { // handle FLAG_ZR
        new_val |= FLAG_ZR;
    }

    if (cpu->reg[reg1] == val2) { // handle FLAG_EQ and FLAG_LT
        new_val |= FLAG_EQ; // eq bit
        new_val &= ~FLAG_LT; // clear lt bit
    } else if (cpu->reg[reg1] < val2) {
        new_val |= FLAG_LT; // lt bit
        new_val &= ~FLAG_EQ; // clear eq bit
    } else {
        new_val &= ~(FLAG_EQ | FLAG_LT); // clear lteq bits
    }
    cpu->reg[5] = new_val;
    cpu->PC++;
}

    /*
    ADC ra $imm8
    2 bytes
    --

    ADC ra rb
    2 bytes
    --

    ra += arg2, sets carry flag if ra + arg2 > 255
    */
void ADC(struct CPU *cpu) {
    uint8_t reg1 = reg_mask(cpu->MEM[cpu->PC]);
    uint8_t val2;
    if (modifier_mask(cpu->MEM[cpu->PC])) {
        cpu->PC++;
        val2 = cpu->reg[reg_mask(cpu->MEM[cpu->PC])];
    } else {
        cpu->PC++;
        val2 = cpu->MEM[cpu->PC];
    }

    uint16_t sum = cpu->reg[reg1] + val2 + ((cpu->reg[5] & FLAG_CR) ? 1 : 0);
    cpu->reg[reg1] = (uint8_t)sum;
    cpu->reg[5] = (sum > 0xFF) ? (cpu->reg[5] | FLAG_CR) : (cpu->reg[5] & ~FLAG_CR);

    cpu->PC++;
}

    /*
    SBB ra $imm8
    2 bytes
    --

    SBB ra rb
    2 bytes
    --

    ra += ~arg2 + 1 if carry not set, ra += ~arg2 if set
    if difference underflows, set carry bit to 1, otherwise 0

    */
void SBB(struct CPU *cpu) {
    uint8_t reg1 = reg_mask(cpu->MEM[cpu->PC]);
    uint8_t val2;

    if (modifier_mask(cpu->MEM[cpu->PC])) {
        cpu->PC++;
        val2 = cpu->reg[reg_mask(cpu->MEM[cpu->PC])];
    } else {
        cpu->PC++;
        val2 = cpu->MEM[cpu->PC];
    }

    uint16_t diff = cpu->reg[reg1] - val2 - ((cpu->reg[5] & FLAG_CR) ? 1 : 0);
    cpu->reg[reg1] = (uint8_t)diff;

    cpu->reg[5] = (diff > 0xFF) ? (cpu->reg[5] | FLAG_CR) : (cpu->reg[5] & ~FLAG_CR);

    cpu->PC++;
}

    /*
    AND ra $imm8
    2 bytes
    --

    AND ra rb
    2 bytes
    --

    ra &= arg2
    */
void AND(struct CPU *cpu) {
    uint8_t reg1 = reg_mask(cpu->MEM[cpu->PC]);
    uint8_t val2;
    if (modifier_mask(cpu->MEM[cpu->PC])) {
        cpu->PC++;
        val2 = cpu->reg[reg_mask(cpu->MEM[cpu->PC])];

    } else {
        cpu->PC++;
        val2 = cpu->MEM[cpu->PC];
    }
    cpu->reg[reg1] &= val2;
    cpu->PC++;
}

    /*
    XOR ra $imm8
    2 bytes
    --

    XOR ra rb
    2 bytes
    --

    ra ^= arg2
    */
void XOR(struct CPU *cpu) {
    uint8_t reg1 = reg_mask(cpu->MEM[cpu->PC]);
    uint8_t val2;
    if (modifier_mask(cpu->MEM[cpu->PC])) {
        cpu->PC++;
        val2 = cpu->reg[reg_mask(cpu->MEM[cpu->PC])];

    } else {
        cpu->PC++;
        val2 = cpu->MEM[cpu->PC];
    }
    cpu->reg[reg1] ^= val2;
    cpu->PC++;
}

    /*
    PUSH $imm8
    2 bytes
    --

    PUSH ra
    1 byte
    --

    pushes arg1 to stack
    */
void PUSH(struct CPU *cpu) {
    if (!(DEFAULT_SP - cpu->SP >= STACK_SIZE)) {
        cpu->SP -= 1; 
        if (modifier_mask(cpu->MEM[cpu->PC])) {
            uint8_t reg1 = reg_mask(cpu->MEM[cpu->PC]);
            cpu->MEM[cpu->SP] = cpu->reg[reg1];
            cpu->PC++;
        } else {
            cpu->PC++;
            cpu->MEM[cpu->SP] = cpu->MEM[cpu->PC];
            cpu->PC++;
        }
    }

}

    /*
    POP
    1 byte
    --

    POP ra
    1 byte
    --

    pops from stack and stores in ra if modifier bit set, otherwise discards
    */
void POP(struct CPU *cpu) {
    
    if (modifier_mask(cpu->MEM[cpu->PC])) {
        uint8_t reg1 = reg_mask(cpu->MEM[cpu->PC]);
        cpu->reg[reg1] = cpu->MEM[cpu->SP];
    }
    if (cpu->SP != DEFAULT_SP) {
        cpu->SP += 1;
    }
    cpu->PC++;
}

    /*
    HLT
    1 byte
    stops clock

    SHR ra
    1 byte
    shifts ra right, sets cr flag if lsb set
    */
void HLT(struct CPU *cpu) {
    if (modifier_mask(cpu->MEM[cpu->PC])) { // shr
        uint8_t reg1 = reg_mask(cpu->MEM[cpu->PC]);
        if (cpu->reg[reg1] & 0b1) { // if lsb is set, set carry flag
            cpu->reg[5] |= FLAG_CR;
        }
        cpu->reg[reg1] >>= 1;
        //printf("cpu: %02X\n", cpu->reg[reg1]);
        cpu->PC++;
    } else {
        print_CPU_state(cpu, 0);
        exit(0);
    }
    
}