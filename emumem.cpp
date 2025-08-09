#include "emumem.h"
#include "string.h"

static u8 ram[4096] = { 0 };

//¶Á ¿¨rom µ½ ÄÚ´æmem
void mem_write_rom(u16 address, const u8* data, size_t size) {
    memcpy(&ram[address], data, size);
}

void mem_write_font(u16 address, const u8* data, size_t size) {
    memcpy(&ram[address], data, size);
}

u8 mem_read(u16 address) {
    return ram[address];
}

void mem_write(u16 address, u8 value) {
    ram[address] = value;
}