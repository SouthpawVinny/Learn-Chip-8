#define SDL_MAIN_HANDLED
#include  "emu.h"
#include  "cart.h"
#include  "ppu.h"
#include  "cpu.h"
#include  "timer.h"
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

    // frame control
    const int TARGET_FPS = 60;
    const int FRAME_DELAY = 1000 / TARGET_FPS;
    Uint32 frameStart;
    int frameTime;

    // timer
    Uint32 lastTimerUpdate = SDL_GetTicks();
    const int TIMER_FREQUENCY = 60; // 60Hz timer

    while (ctx.running) {

        frameStart = SDL_GetTicks();

        handle_input();

        if (ctx.paused) {
            delay(10);
            continue;
        }

        if (!cpu_step()) {
            printf("CPU Stopped\n");
            return -3;
        }

        //get time
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastTimerUpdate >= 1000 / TIMER_FREQUENCY) {
            timer_tick();
            lastTimerUpdate = currentTime;
        }

        //draw
        if (cpu_get_ctx()->is_draw) {
            ppu_screen_update();
            cpu_get_ctx()->is_draw = false; // redraw
        }

        //delay
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    // quit clean
    SDL_Quit();
    TTF_Quit();

    return 0;

}
