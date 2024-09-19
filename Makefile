CC = gcc
CFLAGS = -Wall -std=c99
CPU_LIBS = -Itools/SDL2/include -Ltools/SDL2/lib -lmingw32 -lSDL2main -lSDL2

SRC_CPU = src/cpu/cpu.c
SRC_ASSEMBLER = src/assembler/assembler.c
SRC_DEASSEMBLER = src/deassembler/deassembler.c

BUILD_DIR = output
INPUT_FILE = programs/main.vw
OUTPUT_FILE = output.bin
FILESYSTEM = filesystem.bin
DUMP_FILE = dump.vw

all: custom_cpu_display assembler deassembler run

custom_cpu_display: $(SRC_CPU)
	$(CC) $(CFLAGS) $(SRC_CPU) $(CPU_LIBS) -o $(BUILD_DIR)/cpu

assembler: $(SRC_ASSEMBLER)
	$(CC) $(CFLAGS) $(SRC_ASSEMBLER) -o $(BUILD_DIR)/assembler

deassembler: $(SRC_DEASSEMBLER)
	$(CC) $(CFLAGS) $(SRC_DEASSEMBLER) -o $(BUILD_DIR)/deassembler

run:
	$(BUILD_DIR)/assembler.exe $(INPUT_FILE) $(BUILD_DIR)/$(OUTPUT_FILE)
	$(BUILD_DIR)/cpu.exe $(BUILD_DIR)/$(OUTPUT_FILE)

run_assembler:
	$(BUILD_DIR)/assembler.exe $(INPUT_FILE) $(BUILD_DIR)/$(OUTPUT_FILE)

run_cpu:
	$(BUILD_DIR)/cpu.exe $(BUILD_DIR)/$(OUTPUT_FILE) $(BUILD_DIR)/$(FILESYSTEM)

run_deassembler:
	$(BUILD_DIR)/deassembler.exe $(BUILD_DIR)/$(OUTPUT_FILE) $(BUILD_DIR)/$(DUMP_FILE)

run_all:
	$(BUILD_DIR)/assembler.exe $(INPUT_FILE) $(BUILD_DIR)/$(OUTPUT_FILE)
	$(BUILD_DIR)/deassembler.exe $(BUILD_DIR)/$(OUTPUT_FILE) $(BUILD_DIR)/$(DUMP_FILE)
	$(BUILD_DIR)/cpu.exe $(BUILD_DIR)/$(OUTPUT_FILE) $(BUILD_DIR)/$(FILESYSTEM)