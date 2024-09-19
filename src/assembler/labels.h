#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char label_name[256];
    int low;
    int high;
} LabelIndex;

typedef struct {
    char label_name[256];
    int address;
} Label;

Label labels[256];
int num_labels = 0;

LabelIndex label_indices[256];
int num_label_indices = 0;

/*
convert ABCDZFHL to register #
*/
void register_direct(char* arg) {
    switch (arg[0]) {
        case 'A':
            arg[0] = 'r';
            arg[1] = '0';
            break;
        case 'B':
            arg[0] = 'r';
            arg[1] = '1';
            break;
        case 'C':
            arg[0] = 'r';
            arg[1] = '2';
            break;
        case 'D':
            arg[0] = 'r';
            arg[1] = '3';
            break;
        case 'Z':
            arg[0] = 'r';
            arg[1] = '4';
            break;
        case 'F':
            arg[0] = 'r';
            arg[1] = '5';
            break;
        case 'H':
            arg[0] = 'r';
            arg[1] = '6';
            break;
        case 'L':
            arg[0] = 'r';
            arg[1] = '7';
            break;
    }
}