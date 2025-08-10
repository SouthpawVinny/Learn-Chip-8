#pragma once

#include "common.h"

extern u8 delay_timer;
extern u8 sound_timer;

void timer_init();
void timer_tick();