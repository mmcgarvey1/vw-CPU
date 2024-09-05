CC = gcc
CFLAGS = -Wall -std=c99
CPU_LIBS = -Itools/SDL2/include -Ltools/SDL2/lib -lmingw32 -lSDL2main -lSDL2

SRC_CPU = src/cpu/cpu.c
SRC_COMPILER = src/compiler/compiler.c
SRC_DECOMPILER = src/decompiler/decompiler.c

BUILD_DIR = output
INPUT_FILE = programs/main.vw
OUTPUT_FILE = output.bin
FILESYSTEM = filesystem.bin
DUMP_FILE = dump.vw

all: custom_cpu_display compiler decompiler run

custom_cpu_display: $(SRC_CPU)
	$(CC) $(CFLAGS) $(SRC_CPU) $(CPU_LIBS) -o $(BUILD_DIR)/cpu

compiler: $(SRC_COMPILER)
	$(CC) $(CFLAGS) $(SRC_COMPILER) -o $(BUILD_DIR)/compiler

decompiler: $(SRC_DECOMPILER)
	$(CC) $(CFLAGS) $(SRC_DECOMPILER) -o $(BUILD_DIR)/decompiler

run:
	$(BUILD_DIR)/compiler.exe $(INPUT_FILE) $(BUILD_DIR)/$(OUTPUT_FILE)
	$(BUILD_DIR)/cpu.exe $(BUILD_DIR)/$(OUTPUT_FILE)

run_compiler:
	$(BUILD_DIR)/compiler.exe $(INPUT_FILE) $(BUILD_DIR)/$(OUTPUT_FILE)

run_cpu:
	$(BUILD_DIR)/cpu.exe $(BUILD_DIR)/$(OUTPUT_FILE) $(BUILD_DIR)/$(FILESYSTEM)

run_decompiler:
	$(BUILD_DIR)/decompiler.exe $(BUILD_DIR)/$(OUTPUT_FILE) $(BUILD_DIR)/$(DUMP_FILE)

run_all:
	$(BUILD_DIR)/compiler.exe $(INPUT_FILE) $(BUILD_DIR)/$(OUTPUT_FILE)
	$(BUILD_DIR)/decompiler.exe $(BUILD_DIR)/$(OUTPUT_FILE) $(BUILD_DIR)/$(DUMP_FILE)
	$(BUILD_DIR)/cpu.exe $(BUILD_DIR)/$(OUTPUT_FILE) $(BUILD_DIR)/$(FILESYSTEM)