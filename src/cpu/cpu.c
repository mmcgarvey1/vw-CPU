#include <stdio.h>
#include <stdlib.h>
#include "keyboard.h"

uint32_t COLORS[] = {BLACK, GRAY, LIGHT_GRAY, WHITE};

void update_colors(const uint8_t *memory, uint32_t *video_memory, size_t count) {
    for (size_t i = 0; i < count; i++) {
        uint8_t data = memory[i];
        for (int j = 0; j < 4; j++) {
            int bit_position = 6 - (2 * j);
            uint8_t val = (data >> bit_position) & 0b11;
            video_memory[i * 4 + j] = COLORS[val];
        }
    }
}

void update_memory(SDL_Renderer *renderer, SDL_Texture *texture, uint32_t *video_memory, const uint8_t *memory, const uint8_t segment, int highlight) {
    void *pixels;
    int pitch;
    if (SDL_LockTexture(texture, NULL, &pixels, &pitch) != 0) {
        fprintf(stderr, "Error locking texture: %s\n", SDL_GetError());
        exit(1);
    }
    for (size_t i = 0; i < CAPACITY; i++) {
        uint8_t brightness = memory[i];
        uint32_t color;

        if (segment == 1 && brightness == 0) {
            if (i <= ROM_END) {                                                                             
                color = SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 0, 0, 0, 255);               
            
            } else if ((i >= VIDEO_START) && (i < VIDEO_START + 0x1000)) {                                  // video memory
                color = SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 235, 55, 52, 255);
            
            } else if ((i >= VIDEO_START + 0x1000) && (i < VIDEO_START + 0x2000)) {                         // character table
                color = SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 0, 0, 0, 255);
            
            } else if ((i >= VIDEO_START + 0x2000) && (i < VIDEO_START + 0x2100)) {                         // character buffer
                color = SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 41, 227, 41, 255);
            
            } else if ((i >= VIDEO_START + 0x2100) && (i < VIDEO_START + 0x2200)) {                         // display box
                color = SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 0, 0, 0, 255);

            } else if ((i >= VIDEO_START + 0x2209) && (i < VIDEO_START + 0x2C00)) {                         // file allocation table
                color = SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 128, 0, 0, 255);

            } else if ((i >= VIDEO_START + 0x2C00) && (i < VIDEO_START + 0x4000)) {                         // unused
                color = SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 223, 100, 223, 255);
            
            } else if ((i >= DEFAULT_SP - STACK_SIZE) && (i < DEFAULT_SP)) {                                // stack
                color = SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 56, 56, 227, 255);
            
            } else {
                color = SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 255, 0, 0, 255);
            }
            
        } else {
            if (highlight == i) {
                color = SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 0, 0, 255, 255);
            } else {
                color = SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), brightness, brightness, brightness, 255);
            }
        }

        ((uint32_t*)pixels)[i] = color;
    }
    SDL_UnlockTexture(texture);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void update_display(SDL_Renderer *renderer, SDL_Texture *texture, uint32_t *video_memory) {
    void *pixels;
    int pitch;
    if (SDL_LockTexture(texture, NULL, &pixels, &pitch) != 0) {
        fprintf(stderr, "Error locking texture: %s\n", SDL_GetError());
        exit(1);
    }
    memcpy(pixels, video_memory, WIDTH * HEIGHT * sizeof(uint32_t));
    SDL_UnlockTexture(texture);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}


int main(int argc, char *argv[]) {
    printf("\033[2J\033[H"); // clear terminal
    
    struct CPU cpu;
    init_CPU(&cpu);
    load_bin(&cpu, argv[1], CAPACITY);
    
    cpu.DEVICE[KEYBOARD_DEV] = 255;

    int display_frame = 0; // cycle counter
    uint32_t video_memory[WIDTH * HEIGHT];
    uint32_t memory_video_memory[CAPACITY + WIDTH_MEM];
    uint32_t fs_video_memory[CAPACITY];
    uint8_t fs_memory[CAPACITY];

    SDL_Window *window_3 = SDL_CreateWindow("DISK", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH_DISK * PIXEL_SIZE_DISK, HEIGHT_DISK * PIXEL_SIZE_DISK, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer_3 = SDL_CreateRenderer(window_3, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture_3 = SDL_CreateTexture(renderer_3, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_STREAMING, WIDTH_DISK, HEIGHT_DISK);

    SDL_Window *window_2 = SDL_CreateWindow("MEM", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH_MEM * PIXEL_SIZE_MEM, HEIGHT_MEM * PIXEL_SIZE_MEM, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer_2 = SDL_CreateRenderer(window_2, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture_2 = SDL_CreateTexture(renderer_2, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_STREAMING, WIDTH_MEM, HEIGHT_MEM);

    SDL_Window *window = SDL_CreateWindow("CPU", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH * PIXEL_SIZE, HEIGHT * PIXEL_SIZE, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    
    SDL_Event event;
    while (1) {
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                        SDL_DestroyTexture(texture);
                        SDL_DestroyRenderer(renderer);
                        SDL_DestroyWindow(window);
                        SDL_DestroyTexture(texture_2);
                        SDL_DestroyRenderer(renderer_2);
                        SDL_DestroyWindow(window_2);
                        SDL_DestroyTexture(texture_3);
                        SDL_DestroyRenderer(renderer_3);
                        SDL_DestroyWindow(window_3);
                        SDL_Quit();
                        print_CPU_state(&cpu, 0);
                        return 0;
                    }
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.scancode < SDL_NUM_SCANCODES) {
                        if (event.key.keysym.mod & KMOD_SHIFT) {
                            cpu.DEVICE[KEYBOARD_DEV] = get_key_code(shift_convert(event.key.keysym.sym));
                        } else if (event.key.keysym.mod & KMOD_CTRL) {
                            cpu.DEVICE[KEYBOARD_DEV] = ctrl_convert(event.key.keysym.sym);
                        } else {
                            cpu.DEVICE[KEYBOARD_DEV] = get_key_code(event.key.keysym.sym);
                        }
                    }
                    break;
                case SDL_KEYUP:
                    cpu.DEVICE[KEYBOARD_DEV] = 255;
                    break;
                default:
                    break;
            }
        } else {
            execute_instruction(&cpu);
            display_frame++;
            if (display_frame == INST_PER_FRAME) {
                update_colors(cpu.MEM + VIDEO_START, video_memory, WIDTH * HEIGHT / 4);
                update_display(renderer, texture, video_memory);
                update_memory(renderer_2, texture_2, memory_video_memory, cpu.MEM, 1, cpu.PC);
                for (int i = 0; i < MAX_BLOCKS; i++) {
                    memcpy(fs_memory + i * BLOCK_SIZE, cpu.FS.blocks[i].data, BLOCK_SIZE);
                }
                update_memory(renderer_3, texture_3, fs_video_memory, fs_memory, 0, -1);
                //print_CPU_state(&cpu, 1);
                display_frame = 0;
            }
        }
    }
}

