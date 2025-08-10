#pragma once

#include "common.h"

typedef struct {
    bool paused;
    bool running;
} emu_context;

u8 wait_for_key();

int emu_run(int argc, char** argv);

emu_context* emu_get_context();