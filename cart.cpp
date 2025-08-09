#include  "cart.h"
#include  "cpu.h"
#include  "emumem.h"

#ifdef _WIN32
#include <errno.h>
#endif

typedef struct {
    char filename[1024];
    u32 rom_size;
} cart_context;

static cart_context ctx;

bool cart_load(char* cart) {
    snprintf(ctx.filename, sizeof(ctx.filename), "%s", cart);

#ifdef _WIN32
    FILE* fp = NULL;
    if (fopen_s(&fp, cart, "rb") != 0 || fp == NULL) {
        printf("Failed to open: %s\n", cart);
        return false;
    }
#else
    FILE* fp = fopen(cart, "rb");
    if (!fp) {
        printf("Failed to open: %s\n", cart);
        return false;
    }
#endif

    printf("Opened: %s\n", ctx.filename);

    fseek(fp, 0, SEEK_END);
    ctx.rom_size = ftell(fp);
    rewind(fp);

    u8* rom_data = (u8*)malloc(ctx.rom_size);
    fread(rom_data, ctx.rom_size, 1, fp);
    fclose(fp);
    mem_write_rom(PROGRAM_ENTER_POINT, rom_data, ctx.rom_size);

    printf("Cartridge Loaded: \n");

    return true;
}
