// DEBUG
#define MEMORY_ROWS         8
#define STACK_ROWS          16

// MEMORY
#define CAPACITY            0x10000
#define DEFAULT_SP          0xFFFF
#define DEFAULT_PC          0x0000
#define REGISTER_COUNT      0x8
#define DEVICE_COUNT        0x100
#define STACK_SIZE          0XFF
#define ROM_END             0x7FFF
#define KEYBOARD_DEV        0x00
#define LINE_OUTPUT         0x01

// INTERRUPTS

#define INT_READ            0x00
#define INT_WRITE           0x01

// VIDEO
#define WIDTH               128
#define HEIGHT              128
#define VIDEO_START         0xA000
#define PIXEL_SIZE          2

#define WIDTH_MEM           256
#define HEIGHT_MEM          256
#define PIXEL_SIZE_MEM      1

#define WIDTH_DISK          256
#define HEIGHT_DISK         256
#define PIXEL_SIZE_DISK     1

#define BLACK               0x000000FF
#define GRAY                0x808080FF
#define LIGHT_GRAY          0xC0C0C0FF
#define WHITE               0xFFFFFFFF

#define INST_PER_FRAME      65535


// FLAG REGISTER r5:
#define FLAG_LT             0b10000000 // less than
#define FLAG_EQ             0b01000000 // equal
#define FLAG_ZR             0b00100000 // zero
#define FLAG_CR             0b00010000 // carry
#define FLAG_UN1            0b00001000 // unused
#define FLAG_UN2            0b00000100 // unused
#define FLAG_UN3            0b00000010 // unused
#define FLAG_UN4            0b00000001 // unused

// OPCODES
#define OP_NUL              0x00
#define OP_MW               0x10
#define OP_LW               0x20
#define OP_SW               0x30
#define OP_LPC              0x40
#define OP_IN               0x50
#define OP_OUT              0x60
#define OP_JNZ              0x70
#define OP_CMP              0x80
#define OP_ADC              0x90
#define OP_SBB              0xA0
#define OP_AND              0xB0
#define OP_XOR              0xC0
#define OP_PUSH             0xD0
#define OP_POP              0xE0
#define OP_HLT              0xF0

// COMPILATION
#define FLAG_R              0b00001000
#define VAR_TABLE_SIZE      256

// REGISTERS
#define rA                  "r0"
#define rB                  "r1"
#define rC                  "r2"
#define rD                  "r3"
#define rZ                  "r4"
#define rF                  "r5"
#define rH                  "r6"
#define rL                  "r7"
#define REGISTER_NAMES      "ABCDZFHL"

// FILESYSTEM

#define BLOCK_SIZE          0x100
#define MAX_BLOCKS          0x100

#define FLAG_SUCCESS        0b1000
#define FLAG_BLOCKSIZE      0b0100
#define FLAG_EXISTS         0b0010
#define FLAG_UNUSED         0b0001