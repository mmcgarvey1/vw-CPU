#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../shared/constants.h"


uint8_t process_arg(char* arg, uint8_t* flag_r) {
    uint8_t a1 = 0;
    if (arg[0] == 'r') {
        a1 = arg[1] - '0';
        *flag_r = FLAG_R;
    } else if (arg[0] == '$') {
        a1 = strtol(arg + 1, NULL, 0);
        *flag_r = 0;
    }
    return a1;
}

void write_bytes(uint8_t* memory, int* index, uint8_t* data, int size) {
    for (int i = 0; i < size; i++) {
        memory[(*index)++] = data[i];
    }
}

void gb(uint8_t* memory, int* index, char* instruction, char* arg1, char* arg2) {
    uint8_t flag_r = 0;
    uint8_t a1 = process_arg(arg1, &flag_r);
    uint8_t a2 = process_arg(arg2, &flag_r);

    if (strcmp(instruction, "NUL") == 0) {
        uint8_t binary[] = { OP_NUL };
        write_bytes(memory, index, binary, sizeof(binary));
    } else if (strcmp(instruction, "INT") == 0) {
        uint8_t binary[] = { OP_NUL | FLAG_R | (a1 & (0b111))}; // mask all but lower 3 bits
        write_bytes(memory, index, binary, sizeof(binary));
    } else if (strcmp(instruction, "MW") == 0) {
        uint8_t binary[] = { OP_MW | flag_r | a1, a2 };
        write_bytes(memory, index, binary, sizeof(binary));
    } else if (strcmp(instruction, "LW") == 0) {
        if (flag_r) {
            uint8_t binary[] = { OP_LW | flag_r | a1, a2 };
            write_bytes(memory, index, binary, sizeof(binary));
        } else {
            uint16_t address = (uint16_t)strtol(arg2 + 1, NULL, 0);
            uint8_t binary[] = { OP_LW  | a1, (address & 0xFF00) >> 8, address & 0xFF};
            write_bytes(memory, index, binary, sizeof(binary));
        }
    } else if (strcmp(instruction, "SW") == 0) {
        if (arg1[0] == 'r') {
            uint8_t binary[] = { OP_SW | FLAG_R | a1, a2 };
            write_bytes(memory, index, binary, sizeof(binary));
        } else {
            uint16_t address = (uint16_t)strtol(arg1 + 1, NULL, 0);
            uint8_t binary[] = { OP_SW | a2, (address & 0xFF00) >> 8, address & 0xFF };
            write_bytes(memory, index, binary, sizeof(binary));
        }
    } else if (strcmp(instruction, "LPC") == 0) {
        uint8_t binary[] = { OP_LPC | a1};
        write_bytes(memory, index, binary, sizeof(binary));
    } else if (strcmp(instruction, "CLR") == 0) {
        uint8_t binary[] = {OP_LPC | FLAG_R | a1};
        write_bytes(memory, index, binary, sizeof(binary));
    } else if (strcmp(instruction, "IN") == 0) {
        uint8_t binary[] = { OP_IN | flag_r | a1, a2 };
        write_bytes(memory, index, binary, sizeof(binary));
    } else if (strcmp(instruction, "OUT") == 0) {
        if (arg1[0] == 'r') {
            uint8_t binary[] = { OP_OUT | FLAG_R | a1, a2 };
            write_bytes(memory, index, binary, sizeof(binary));
        } else {
            uint8_t binary[] = { OP_OUT | a2, a1 };
            write_bytes(memory, index, binary, sizeof(binary));
        }
        
    } else if (strcmp(instruction, "_JNZ") == 0) {
        if (flag_r) {
            uint8_t binary[] = { OP_JNZ | flag_r | a1};
            write_bytes(memory, index, binary, sizeof(binary));
        } else {
            uint8_t binary[] = { OP_JNZ | flag_r | 1};
            write_bytes(memory, index, binary, sizeof(binary));
        }
    } else if (strcmp(instruction, "CMP") == 0) {
        uint8_t binary[] = { OP_CMP | flag_r | a1, a2 };
        write_bytes(memory, index, binary, sizeof(binary));
    } else if (strcmp(instruction, "ADC") == 0) {
        uint8_t binary[] = { OP_ADC | flag_r | a1, a2 };
        write_bytes(memory, index, binary, sizeof(binary));
    } else if (strcmp(instruction, "SBB") == 0) {
        uint8_t binary[] = { OP_SBB | flag_r | a1, a2 };
        write_bytes(memory, index, binary, sizeof(binary));
    } else if (strcmp(instruction, "AND") == 0) {
        uint8_t binary[] = { OP_AND | flag_r | a1, a2 };
        write_bytes(memory, index, binary, sizeof(binary));
    } else if (strcmp(instruction, "XOR") == 0) {
        uint8_t binary[] = { OP_XOR | flag_r | a1, a2 };
        write_bytes(memory, index, binary, sizeof(binary));
    } else if (strcmp(instruction, "PUSH") == 0) {
        uint8_t binary[] = { OP_PUSH | flag_r | a1};
        write_bytes(memory, index, binary, sizeof(binary));
    } else if (strcmp(instruction, "POP") == 0) {
        if (flag_r) {
            uint8_t binary[] = { OP_POP | flag_r | a1 };
            write_bytes(memory, index, binary, sizeof(binary));
        } else {
            uint8_t binary[] = { OP_POP };
            write_bytes(memory, index, binary, sizeof(binary));
        }
    } else if (strcmp(instruction, "HLT") == 0) {
        uint8_t binary[] = {OP_HLT};
        write_bytes(memory, index, binary, sizeof(binary));
    } else if (strcmp(instruction, "SHR") == 0) {
        uint8_t binary[] = {OP_HLT | FLAG_R | a1};
        write_bytes(memory, index, binary, sizeof(binary));
    } else {
        printf("Error: Unrecognized instruction %s\n", instruction);
    }
}

