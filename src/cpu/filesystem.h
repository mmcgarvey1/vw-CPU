#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../shared/constants.h"

#define FILE_SYSTEM_SIZE (sizeof(struct FileSystem))

struct Block {
    uint8_t data[BLOCK_SIZE];
    uint8_t in_use;
};

struct FileSystem {
    struct Block blocks[MAX_BLOCKS];
    uint8_t flags;
};

void fs_init(struct FileSystem *fs) {
    memset(fs->blocks, 0, sizeof(fs->blocks));
    fs->flags = 0;
}

uint8_t fs_create_file(struct FileSystem *fs) { // returns file index
    for (uint8_t i = 0; i < MAX_BLOCKS - 1; i++) {
        if (fs->blocks[i].in_use == 0) {
            fs->blocks[i].in_use = 1;
            return i;
        }
    }
    return 0xFF;

}

void fs_write_data(struct FileSystem *fs, uint8_t block_index, uint8_t byte_index, uint8_t byte_data, uint8_t flags) {
    if (flags & 0b0010) {
        fs->blocks[block_index].in_use = 0;
    } else {
        fs->flags = 0b1000;
        fs->blocks[block_index].data[byte_index] = byte_data;
    }
    
}

uint8_t fs_read_data(struct FileSystem *fs, uint8_t block_index, uint8_t byte_index) {
    if (fs->blocks[block_index].in_use == 0) {
        fs->flags = 0b0010;
        return 0xFF;
    } else {
        fs->flags = 0b1000;
        return fs->blocks[block_index].data[byte_index];
    }
}