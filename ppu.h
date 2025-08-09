#pragma once

#define SDL_MAIN_HANDLED
#include "common.h"
#include  "SDL.h"
#include  "SDL_ttf.h"


#define PPU_SCREEN_WIDTH 64
#define PPU_SCREEN_HIGHT 32
#define PPU_SCREEN_SIZE 64*32


typedef struct {
    bool display_pixel[PPU_SCREEN_SIZE];
    u32 fg_color;
    u32 bg_color;

    u16 scale;

    SDL_Window* sdl_window;
    SDL_Renderer* sdl_renderer;
} ppu_context;

ppu_context* ppu_get_ctx();
void ppu_init();
void ppu_tick();
void ppu_screen_clean();
void ppu_set_pixel(u16 address);
void ppu_screen_update();