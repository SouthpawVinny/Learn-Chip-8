#include "timer.h"

u8 delay_timer = 0;
u8 sound_timer = 0;

void timer_init() {
    delay_timer = 0;
    sound_timer = 0;
}

void timer_tick() {
    if (delay_timer > 0) delay_timer--;
    if (sound_timer > 0) sound_timer--;
}