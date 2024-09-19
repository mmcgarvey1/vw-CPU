#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char variable_name[256];
    char value[8];
} Variable;

Variable variable_table[VAR_TABLE_SIZE];
unsigned char variable_index = 0;