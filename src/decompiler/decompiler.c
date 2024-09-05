#include "../cpu/instructions.h"

void reg_const(FILE *file, uint8_t *memory, int *address) {
    fprintf(file, "r%d ", reg_mask(memory[*address]));
    if (modifier_mask(memory[*address])) {
        (*address)++;
        fprintf(file, "r%d\n", reg_mask(memory[*address]));
    } else {
        (*address)++;
        fprintf(file, "$%02X\n", memory[*address]);
    }
    (*address)++;
}

void execute_instruction(FILE *file, uint8_t *memory, int *address) {
    switch (opcode_mask(memory[*address])) {
        case 0b0000:
            fprintf(file, "NUL\n");
            (*address)++;
            break;
        case 0b0001:
            fprintf(file, "MW ");
            reg_const(file, memory, address);
            break;
        case 0b0010:
            fprintf(file, "LW ");
            fprintf(file, "r%d ", reg_mask(memory[*address]));
            if (modifier_mask(memory[*address])) {
                (*address)++;
                fprintf(file, "r%d\n", modifier_mask(memory[*address]));
                (*address)++;
            } else {
                (*address)++;
                fprintf(file, "$%02X%02X\n", memory[*address], memory[(*address) + 1]);
                (*address)++;
                (*address)++;
            }
            break;
        case 0b0011:
            fprintf(file, "SW ");
            if (modifier_mask(memory[*address])) {
                fprintf(file, "r%d ", reg_mask(memory[*address]));
                (*address)++;
                fprintf(file, "r%d\n", reg_mask(memory[*address]));
                (*address)++;
            } else {
                uint8_t reg = reg_mask(memory[*address]);
                (*address)++;
                fprintf(file, "$%02X%02X ", memory[*address], memory[(*address) + 1]);
                (*address)++;
                (*address)++;
                fprintf(file, "r%d\n", reg);
            }
            break;
        case 0b0100:
        if (modifier_mask(memory[*address])) {
            fprintf(file, "CLR r%d\n", reg_mask(memory[*address]));
            (*address)++;
            break;
        } else {
            fprintf(file, "LPC r%d\n", reg_mask(memory[*address]));
            (*address)++;
            break;
        }
            
        case 0b0101:
            fprintf(file, "IN ");
            reg_const(file, memory, address);
            break;
        case 0b0110:
            fprintf(file, "OUT ");
            if (modifier_mask(memory[*address])) {
                fprintf(file, "r%d ", reg_mask(memory[*address]));
                (*address)++;
                fprintf(file, "r%d\n", reg_mask(memory[*address]));
                (*address)++;
            } else {
                uint8_t reg = reg_mask(memory[*address]);
                (*address)++;
                fprintf(file, "$%02X", memory[*address]);
                fprintf(file, "r%d\n", reg);
                (*address)++;
            }
            break;
        case 0b0111:
            fprintf(file, "_JNZ ");
            if (modifier_mask(memory[*address])) {
                fprintf(file, "r%d\n", reg_mask(memory[*address]));
                (*address)++;
            } else {
                fprintf(file, "$%02X\n", reg_mask(memory[*address]));
                (*address)++;
            }
            break;
        case 0b1000:
            fprintf(file, "CMP ");
            reg_const(file, memory, address);
            break;
        case 0b1001:
            fprintf(file, "ADC ");
            reg_const(file, memory, address);
            break;
        case 0b1010:
            fprintf(file, "SBB ");
            reg_const(file, memory, address);
            break;
        case 0b1011:
            fprintf(file, "AND ");
            reg_const(file, memory, address);
            break;
        case 0b1100:
            fprintf(file, "XOR ");
            reg_const(file, memory, address);
            break;
        case 0b1101:
            fprintf(file, "PUSH ");
            if (modifier_mask(memory[*address])) {
                fprintf(file, "r%d\n", reg_mask(memory[*address]));
                (*address)++;
            } else {
                (*address)++;
                fprintf(file, "$%02X\n", memory[*address]);
                (*address)++;
            }
            break;
        case 0b1110:
            fprintf(file, "POP ");
            if (modifier_mask(memory[*address])) {
                fprintf(file, "r%d\n", reg_mask(memory[*address]));
            } else {
                fprintf(file, "\n");
            }
            (*address)++;
            break;
        case 0b1111:
            if (!modifier_mask(memory[*address])) {
                fprintf(file, "HLT\n");
                (*address)++;
            } else {
                fprintf(file, "SHR r%d\n", reg_mask(memory[*address]));
                (*address)++;
            }
            break;
        default:
            break; 
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <binary> <output>\n", argv[0]);
        return 1;
    }
    FILE *file = fopen(argv[2], "w");
    if (file == NULL) {
        printf("Error opening %s\n", argv[2]);
        return 1;
    }
    struct CPU cpu;
    init_CPU(&cpu);


    load_bin(&cpu, argv[1], CAPACITY);
    for (int i = 0; i < CAPACITY - 1;) {
        fprintf(file, "0x%04X: ", i);
        execute_instruction(file, cpu.MEM, &i);
    }
}
