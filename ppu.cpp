#define SDL_MAIN_HANDLED
#include "ppu.h"
#include "cpu.h"
#include  "emumem.h"
#include  "SDL.h"
static ppu_context ctx = { 0 };


static u8 font[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

ppu_context* ppu_get_ctx() {
    return &ctx;
}

void ppu_init() {
    mem_write_font(FONT_ADDRESS, font, sizeof font);
    cpu_get_ctx()->reg.I = FONT_ADDRESS;
    ctx.fg_color  = 0xFFFFFFFF;
    ctx.bg_color = 0x000000FF;
    ctx.scale = 10;

    // SDL render  loop
    // 
    // 1.SDL init
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL init fail\n");
        exit(-1);
    }
    // 2.Create Window

    ctx.sdl_window = SDL_CreateWindow("CHIP8",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        PPU_SCREEN_WIDTH * ctx.scale, PPU_SCREEN_HIGHT * ctx.scale,
        SDL_WINDOW_SHOWN);

    if (!ctx.sdl_window) {
        printf("SDL Create window fail\n");
        exit(-1);
    }
    // 3.Create Render by Window

    ctx.sdl_renderer = SDL_CreateRenderer(ctx.sdl_window, -1, SDL_RENDERER_ACCELERATED);
    if (!ctx.sdl_renderer) {
        printf("SDL Create renderer fail\n");
        exit(-1);
    }
    // 4.Make Colorbackground color0xFFFFFFFF, front color0x000000FF);

    u32 r = (ctx.bg_color >> 24) & 0xFF;
    u32 g = (ctx.bg_color >> 16) & 0xFF;
    u32 b = (ctx.bg_color >> 8) & 0xFF;
    u32 a = (ctx.bg_color >> 0) & 0xFF;
    // 5.Draw Rect, Rect Fill;

    SDL_SetRenderDrawColor(ctx.sdl_renderer, r, g, b, a);
    SDL_RenderClear(ctx.sdl_renderer);
    // 6.Display
    SDL_RenderPresent(ctx.sdl_renderer);

}

void ppu_tick() {

}
//screen refresh
void ppu_screen_clear() {
    // handle clear screen
    u32 r = (ctx.bg_color >> 24) & 0xFF;
    u32 g = (ctx.bg_color >> 16) & 0xFF;
    u32 b = (ctx.bg_color >> 8) & 0xFF;
    u32 a = (ctx.bg_color >> 0) & 0xFF;
    
    SDL_SetRenderDrawColor(ctx.sdl_renderer, r, g, b, a);
    SDL_RenderClear(ctx.sdl_renderer);
}

void ppu_screen_clean() {
    memset(ctx.display_pixel, false, sizeof ctx.display_pixel);
}

void ppu_set_pixel(u16 address) {
    ctx.display_pixel[address] = true;
}

// update screen
void ppu_screen_update() {
    // clear first
    ppu_screen_clear();
    
    SDL_Rect rect = {
        rect.x = 0,
        rect.y = 0,
        rect.w = ctx.scale,
        rect.h = ctx.scale
    };

    u32 r = (ctx.fg_color >> 24) & 0xFF;
    u32 g = (ctx.fg_color >> 16) & 0xFF;
    u32 b = (ctx.fg_color >> 8) & 0xFF;
    u32 a = (ctx.fg_color >> 0) & 0xFF;

    int x = 0;
    int y = 0;

    SDL_SetRenderDrawColor(ctx.sdl_renderer, r, g, b, a);

    // fetch pixel
    for (u16 pixel_i = 0; pixel_i < PPU_SCREEN_SIZE; pixel_i++) {
        if (ctx.display_pixel[pixel_i]) {
            x = pixel_i % PPU_SCREEN_WIDTH;
            y = pixel_i / PPU_SCREEN_WIDTH;
            x *= ctx.scale;
            y *= ctx.scale;
            rect.x = x;
            rect.y = y;
            SDL_RenderFillRect(ctx.sdl_renderer, &rect);
        }
    }

    SDL_RenderPresent(ctx.sdl_renderer);
}
