#pragma once

#include "common.h"
#include "string.h"

//write rom data to memory from cartridge
void mem_write_rom(u16 address, const u8* data, size_t size);

//write font data to memory from
void mem_write_font(u16 address, const u8* data, size_t size);

void mem_write(u16 address, u8 value);

u8 mem_read(u16 address);