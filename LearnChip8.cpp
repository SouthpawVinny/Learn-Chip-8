#define SDL_MAIN_HANDLED
#include  "emu.h"
#include  "cart.h"
#include  "ppu.h"
#include  "cpu.h"
#include  "SDL.h"
#include  "SDL_ttf.h"

static emu_context ctx;

emu_context* emu_get_context() {
    return &ctx;
}

void delay(u32 ms) {
    SDL_Delay(ms);
}

int main(int argc, char** argv) {
    // Initialize emu context
    ctx.running = true;
    ctx.paused = false;

    // Run the emulator
    return emu_run(argc, argv);
}

int emu_run(int argc, char** argv) {
    printf("Checking arguments...\n");
    printf("argc: %d\n", argc);
    if (argc < 2) {
        printf("Usage: emu <rom_file>\n");
        return -1;
    }
    printf("argv[0]: %s\n", argv[0] ? argv[0] : "NULL");
    printf("argv[1]: %s\n", argv[1] ? argv[1] : "NULL");


    if (!cart_load(argv[1])) {
        printf("Failed to load rom file: %s\n", argv[1]);
        return -2;
    }

    // PPU init
    ppu_init();

    printf("Cart loaded\n");

    SDL_Init(SDL_INIT_VIDEO);
    printf("SDL initialized\n");
    TTF_Init();
    printf("TTF initialized\n");

    cpu_init();

    while (ctx.running) {
        if (ctx.paused) {
            delay(10);
            continue;
        }

        // get time

        if (!cpu_step()) {
            printf("CPU Stopped\n");
            return -3;
        }

        //get time

        //delay

        //draw

        if (cpu_get_ctx()->is_draw) {
            ppu_screen_update();
        }
    }

    return 0;

}
