#pragma once

#include "common.h"

#define PROGRAM_ENTER_POINT 0x200

typedef struct {
    u16 PC;
    u16 I;
    u8  V[0xF];
} reg_t;

typedef struct {
    reg_t reg;

    u16 stack[20]; // > 16
    u16* ptr_stack;

    bool is_draw;
} cpu_context;

// key check
bool is_key_pressed(u8 key);
u8 wait_for_key();
void handle_input();

cpu_context* cpu_get_ctx();
void cpu_init();
bool cpu_step();
void handle_input();
