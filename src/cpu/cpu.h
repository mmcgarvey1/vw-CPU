#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../shared/constants.h"
#include "filesystem.h"

#define BUFFER_SIZE 256

struct CPU {
    uint8_t reg[REGISTER_COUNT];
    uint16_t PC;
    uint16_t SP;

    uint8_t DEVICE[DEVICE_COUNT];
    uint8_t MEM[CAPACITY];
    struct FileSystem FS;
};

void init_CPU(struct CPU *cpu) {
    if (cpu != NULL) {
        for (size_t i = 0; i < REGISTER_COUNT; ++i) {
            cpu->reg[i] = 0;
        }

        cpu->PC = DEFAULT_PC;
        cpu->SP = DEFAULT_SP;

        for (size_t i = 0; i < DEVICE_COUNT; ++i) {
            cpu->DEVICE[i] = 0;
        }
        for (size_t i = 0; i < CAPACITY; ++i) {
            cpu->MEM[i] = 0;
        }
        fs_init(&(cpu->FS));
    }
}

/*
loads binary into memory from file
*/
void load_bin(struct CPU *cpu, const char* filename, size_t capacity) {
    FILE* file = fopen(filename, "rb");
    if (file != NULL) {
        fread(cpu->MEM, sizeof(uint8_t), capacity, file);
        fclose(file);
    } else {
        perror("Error opening file");
    }
}

/* 
masks out register bits in instruction
*/
uint8_t reg_mask(char mem_byte) {
    return mem_byte & 0b00000111;
}

/* 
masks out opcode bits in instruction
*/
uint8_t opcode_mask(char mem_byte) {
    return (mem_byte & 0b11110000) >> 4;
}

/* 
masks out modifier bit in instruction
*/
uint8_t modifier_mask(char mem_byte) {
    return (mem_byte & 0b00001000) >> 3;
}

void reg_const_c(char *buffer, uint8_t *memory, int *address, int *offset) {
    *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "%c ", REGISTER_NAMES[reg_mask(memory[*address])]);
    if (modifier_mask(memory[*address])) {
        (*address)++;
        *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "%c\n", REGISTER_NAMES[reg_mask(memory[*address])]);
    } else {
        (*address)++;
        *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "$%02X\n", memory[*address]);
    }
    (*address)++;
}

void decompile_instruction(char *buffer, uint8_t *memory, int address, int *offset) {
    switch (opcode_mask(memory[address])) {
        case 0b0000:
            *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "NUL\n");
            (address)++;
            break;
        case 0b0001:
            *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "MW ");
            reg_const_c(buffer, memory, &address, &*offset);
            break;
        case 0b0010:
            *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "LW ");
            *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "%c ", REGISTER_NAMES[reg_mask(memory[address])]);
            if (modifier_mask(memory[address])) {
                (address)++;
                *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "%c\n", REGISTER_NAMES[reg_mask(memory[address])]);
                (address)++;
            } else {
                (address)++;
                *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "$%02X%02X\n", memory[address], memory[(address) + 1]);
                (address)++;
                (address)++;
            }
            break;
        case 0b0011:
            *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "SW ");
            if (modifier_mask(memory[address])) {
                *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "%c ", REGISTER_NAMES[reg_mask(memory[address])]);
                (address)++;
                *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "%c\n", REGISTER_NAMES[reg_mask(memory[address])]);
                (address)++;
            } else {
                uint8_t reg = reg_mask(memory[address]);
                (address)++;
                *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "$%02X%02X ", memory[address], memory[(address) + 1]);
                (address)++;
                (address)++;
                *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "%c\n", REGISTER_NAMES[reg]);
            }
            break;
        case 0b0100:
            if (modifier_mask(memory[address])) {
                *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "CLR %c\n", REGISTER_NAMES[reg_mask(memory[address])]);
            } else {
                *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "LPC %c\n", REGISTER_NAMES[reg_mask(memory[address])]);
            }
            (address)++;
            break;
        case 0b0101:
            *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "IN ");
            reg_const_c(buffer, memory, &address, &*offset);
            break;
        case 0b0110:
            *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "OUT ");
            if (modifier_mask(memory[address])) {
                *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "%c ", REGISTER_NAMES[reg_mask(memory[address])]);
                (address)++;
                *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "%c\n", REGISTER_NAMES[reg_mask(memory[address])]);
                (address)++;
            } else {
                uint8_t reg = reg_mask(memory[address]);
                (address)++;
                *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "$%02X", memory[address]);
                *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "r%d\n", reg);
                (address)++;
            }
            break;
        case 0b0111:
            *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "_JNZ ");
            if (modifier_mask(memory[address])) {
                *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "%c\n", REGISTER_NAMES[reg_mask(memory[address])]);
                (address)++;
            } else {
                *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "$%02X\n", reg_mask(memory[address]));
                (address)++;
            }
            break;
        case 0b1000:
            *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "CMP ");
            reg_const_c(buffer, memory, &address, &*offset);
            break;
        case 0b1001:
            *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "ADC ");
            reg_const_c(buffer, memory, &address, &*offset);
            break;
        case 0b1010:
            *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "SBB ");
            reg_const_c(buffer, memory, &address, &*offset);
            break;
        case 0b1011:
            *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "AND ");
            reg_const_c(buffer, memory, &address, &*offset);
            break;
        case 0b1100:
            *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "XOR ");
            reg_const_c(buffer, memory, &address, &*offset);
            break;
        case 0b1101:
            *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "PUSH ");
            if (modifier_mask(memory[address])) {
                *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "%c\n", REGISTER_NAMES[reg_mask(memory[address])]);
                (address)++;
            } else {
                (address)++;
                *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "$%02X\n", memory[address]);
                (address)++;
            }
            break;
        case 0b1110:
            *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "POP ");
            if (modifier_mask(memory[address])) {
                *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "%c\n", REGISTER_NAMES[reg_mask(memory[address])]);
            } else {
                *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "\n");
            }
            (address)++;
            break;
        case 0b1111:
            if (!modifier_mask(memory[address])) {
                *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "HLT\n");
                (address)++;
            } else {
                *offset += snprintf(buffer + *offset, BUFFER_SIZE - *offset, "SHR %c\n", REGISTER_NAMES[reg_mask(memory[address])]);
                (address)++;
            }
            
            break;
        default:
            break; 
    }
}


/*
prints out the state of the CPU; registers, memory around PC, and stack at SP
*/
void print_CPU_state(struct CPU *cpu, int clear) {
    char buffer[BUFFER_SIZE];
    int offset = 0;

    offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "\n\n");
    for (int i = 0; i < REGISTER_COUNT; i++) {
        offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "%c: 0x%02X\n", REGISTER_NAMES[i], cpu->reg[i]);
    }

    offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "PC: 0x%04X\n", cpu->PC);
    offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "SP: 0x%04X\n", cpu->SP);

    decompile_instruction(buffer, cpu->MEM, cpu->PC, &offset);
    offset--;

    offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "         \n");

    printf("%s", buffer);

    if (clear) printf("\033[%dA", BUFFER_SIZE);
}