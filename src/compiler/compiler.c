#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../shared/constants.h"
#include "labels.h"
#include "binarygen.h"
#include "variables.h"

/*
write data to binary file
*/
void write_bin(const char* filename, const uint8_t* data, size_t size) {
    FILE* file = fopen(filename, "wb");
    if (file != NULL) {
        fwrite(data, sizeof(uint8_t), size, file);
        fclose(file);
    } else {
        perror("Error opening file");
    }
}

/*
create label identifier inside of call/jump instruction
*/
void init_label_director(uint8_t* memory, int* index, const char* function_name) {
    LabelIndex label_index;
    gb(memory, index, "MW", rH, "$0x0");
    label_index.high = *index - 1;
    gb(memory, index, "MW", rL, "$0x0"); 
    label_index.low = *index - 1;
    strcpy(label_index.label_name, function_name);
    label_indices[num_label_indices++] = label_index;
}

/*
define variable in variable table
*/
void define_variable(const char* variable_name, const char* value) {
    Variable variable;
    strcpy(variable.variable_name, variable_name);
    strcpy(variable.value, value);
    variable_table[variable_index] = variable;
    variable_index++;
    printf("variable defined\n");
    printf("%s\n", variable_name);
    printf("%s\n", value);
}

/*
get value of variable from variable table as string
*/
const char* lookup_variable(const char* variable_name) {
    for (int i = 0; i < VAR_TABLE_SIZE; i++) {
        if (strcmp(variable_table[i].variable_name, variable_name) == 0) {
            printf("x");
            printf("%s\n", variable_table[i].value);
            return variable_table[i].value;
        }
    }
    printf("Unknown variable \"%s\"", variable_name);
    return "0";
}

/*
create label at *index
*/
void LBL(uint8_t* memory, int* index, const char* label_name) {
    Label label;
    strcpy(label.label_name, label_name);
    label.address = *index;
    labels[num_labels++] = label;
}

/*
call label, expects RET instruction. 
effective size can vary due to LPC instruction loading the next instruction's address.
*/
void CALL(uint8_t* memory, int* index, const char* function_name) {
    gb(memory, index, "LPC", rH, "");                   // 1 byte,  0 bytes if unconditional
    gb(memory, index, "PUSH", rH, "");                  // 2 bytes, 1 byte if unconditional
    gb(memory, index, "PUSH", rL, "");                  // 3 bytes, 2 bytes if unconditional
    init_label_director(memory, index, function_name);  // 7 bytes, 6 bytes if unconditional
    gb(memory, index, "_JNZ", "$1", "");                // 8 bytes, 7 bytes if unconditional
}

/*
jump to label, does not expect RET instruction
*/
void JMP(uint8_t* memory, int* index, const char* function_name) {
    init_label_director(memory, index, function_name);  // 4 bytes
    gb(memory, index, "_JNZ", "$1", "");                // 5 bytes
}

/*
pop LH from stack, jump 7 bytes from popped address to skip call logic
*/
void RET(uint8_t* memory, int* index) {
    gb(memory, index, "POP", rL, "");
    gb(memory, index, "POP", rH, "");
    gb(memory, index, "AND", rF, "$0xEF");  // clear carry flag
    gb(memory, index, "ADC", rL, "$0x7");   // 7 byte offset from unconditional call
    gb(memory, index, "ADC", rH, "$0");     // carry if needed
    gb(memory, index, "_JNZ", "$1", "");
}

/*
conditional call if F != 0
*/
void CALL_c(uint8_t* memory, int* index, const char* function_name) {
    gb(memory, index, "LPC", rH, "");
    gb(memory, index, "CMP", rF, rF);       // 2 bytes
    gb(memory, index, "ADC", rL, "$0x11");  // 4 bytes (skip 17 bytes if F != 0)
    gb(memory, index, "ADC", rH, "$0");     // 6 bytes
    gb(memory, index, "AND", rF, "$0x20");  // 8 bytes (check if zero flag is set from CMP)
    gb(memory, index, "_JNZ", rF, "");      // 9 bytes
    CALL(memory, index, function_name);     // 9 bytes + 8 bytes -> 0x11 bytes skip if fail CMP
}

/*
conditional jump if F != 0
*/
void JUMP_c(uint8_t* memory, int* index, const char* function_name) {
    gb(memory, index, "LPC", rH, "");
    gb(memory, index, "CMP", rF, rF);       // 2 bytes
    gb(memory, index, "ADC", rL, "$0xE");   // 4 bytes (skip 14 bytes if F != 0)
    gb(memory, index, "ADC", rH, "$0");     // 6 bytes
    gb(memory, index, "AND", rF, "$0x20");  // 8 bytes (check if zero flag is set from CMP)
    gb(memory, index, "_JNZ", rF, "");      // 9 bytes
    JMP(memory, index, function_name);      // 9 bytes + 5 bytes -> 0xE bytes skip if fail CMP
}

/*
resolve label addresses in label table
*/
void resolve_labels(uint8_t* memory) {
    for (int i = 0; i < num_label_indices; i++) {
        for (int j = 0; j < num_labels; j++) {
            
            if (strcmp(label_indices[i].label_name, labels[j].label_name) == 0) {
                printf("label: %s address: %d\n", label_indices[i].label_name, labels[j].address);
                memory[label_indices[i].low] = (uint8_t)(labels[j].address & 0xFF);
                memory[label_indices[i].high] = (uint8_t)((labels[j].address >> 8) & 0xFF);
            }
        }
    }
}

/*
compiles instructions and common macros to binary
*/
void parse_line(const char *line, uint8_t *memory, int *index) {
    char arg1[256] = "";
    char arg2[256] = "";
    char arg3[256] = "";

    sscanf(line, "%s %s %s", arg1, arg2, arg3);

    // check if arg2 and/or arg3 are variables
    char *variable_identifier_arg2 = strchr(arg2, '@'); 
    char *variable_identifier_arg3 = strchr(arg3, '@'); 

    if (variable_identifier_arg2 != NULL) {
        *variable_identifier_arg2 = '\0'; // remove @ from variable name for proper lookup
        strcpy(arg2, lookup_variable(arg2));
    }
    if (variable_identifier_arg3 != NULL) {
        *variable_identifier_arg3 = '\0';
        strcpy(arg3, lookup_variable(arg3));
    }

    char *label_identifier = strchr(arg1, ':'); // if arg1 contains :, remove it, then make sure we know it is a label.
    if (label_identifier != NULL) {
        *label_identifier = '\0';
        LBL(memory, index, arg1);
    } else {
        if (arg1[0] == '\0') {
            return;
        } else if (strcmp(arg1, "static") == 0) {
            define_variable(arg2, arg3);
        } else if (strcmp("JNZ", arg1) == 0) {
            JUMP_c(memory, index, arg2);
        } else if (strcmp("JMP", arg1) == 0) {
            JMP(memory, index, arg2);
        } else if (strcmp("CNZ", arg1) == 0) {
            CALL_c(memory, index, arg2);
        } else if (strcmp("CALL", arg1) == 0) {
            CALL(memory, index, arg2);
        } else if (strcmp("RET", arg1) == 0) {
            RET(memory, index);
        } else if (strcmp("ADD", arg1) == 0) {
            register_direct(arg2);
            register_direct(arg3);
            gb(memory, index, "CLR", rF, "");
            gb(memory, index, "ADC", arg2, arg3);
        } else if (strcmp("SUB", arg1) == 0) {
            register_direct(arg2);
            register_direct(arg3);
            gb(memory, index, "PUSH", rF, "");
            gb(memory, index, "XOR", rF, rF);
            gb(memory, index, "SBB", arg2, arg3);
            gb(memory, index, "POP", rF, "");
        } else if (strcmp("DB", arg1) == 0) {
                uint8_t byte = strtol(arg3 + 1, NULL, 0);
                uint16_t address = strtol(arg2 + 1, NULL, 0);
                memory[address] = byte;
        } else {
            register_direct(arg2);
            register_direct(arg3);
            gb(memory, index, arg1, arg2, arg3);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input file> <output file>\n", argv[0]);
        return 1;
    }
    uint8_t memory[CAPACITY] = {0};
    int index = 0;
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }
    char line[512];
    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        char *comment = strchr(line, ';');
        if (comment != NULL) {
            *comment = '\0';
        }
        parse_line(line, memory, &index);
    }
    fclose(file);
    resolve_labels(memory);
    write_bin(argv[2], memory, CAPACITY);
    return 0;
}
