#include "cpu.h"
#include "ppu.h"
#include "string.h"
#include  "emumem.h"

static cpu_context ctx = { 0 };

cpu_context* cpu_get_ctx() {
    return &ctx;
}

void cpu_init() {
    ctx.reg.PC = PROGRAM_ENTER_POINT;
    ctx.reg.I = 0;
    memset(ctx.reg.V, 0, sizeof(ctx.reg.V));
    memset(ctx.stack, 0, sizeof(ctx.stack));
    ctx.ptr_stack = ctx.stack;
    ctx.is_draw = false;
}

bool cpu_step() {

    ctx.is_draw = false;
    u16 opcode = (mem_read(ctx.reg.PC) << 8) | (mem_read(ctx.reg.PC + 1));
    ctx.reg.PC += 2;

    u8 type = (opcode & 0xF000) >> 12;
    u8 X = (opcode & 0x0F00) >> 8;
    u8 Y = (opcode & 0x00F0) >> 4;
    u8 N = opcode & 0x000F;
    u8 NN = opcode & 0x00FF;
    u16 NNN = opcode & 0x0FFF;

    // decode
    switch (type) {
    case 0x0: //00E0 (clear screen) 
    {
        if (opcode == 0x00E0) {
            // fill zero to screen buffer
            ppu_screen_clean();
        }
        else if (opcode == 0x00EE) {
            ctx.ptr_stack--;
            ctx.reg.PC = *ctx.ptr_stack;
        }
        else if (opcode == NNN) {
            ctx.reg.PC = NNN;
        }
    }
    break;
    case 0x1: //1NNN (jump) 
    {
        ctx.reg.PC = NNN;
    }
    break;
    case 0x2: // Calls subroutine at NNN
    {
        *ctx.ptr_stack = ctx.reg.PC;
        ctx.ptr_stack++;
        ctx.reg.PC = NNN;

    }
    break;
    case 0x3: //3XNN(skip if VX == NN)
    {
        if (ctx.reg.V[X] == NN) ctx.reg.PC += 2;
    }
    break;
    case 0x4: //4XNN(skip if VX != NN)
    {
        if (ctx.reg.V[X] != NN) ctx.reg.PC += 2;
    }
    break;
    case 0x5: //5XY0(skip if VX == VY)
    {
        if (ctx.reg.V[X] == ctx.reg.V[Y]) ctx.reg.PC += 2;
    }
    break;
    case 0x6: //6XNN (set register VX)
    {
        ctx.reg.V[X] = NN;
    }
    break;
    case 0x7: //7XNN (add value to register VX)
    {
        ctx.reg.V[X] += NN;
    }
    break;
    case 0x8: //8XY0(set VX to VY)
        switch (opcode & 0xF) {
        case 0x0: // 8XY0	Assig	Vx = Vy	Sets VX to the value of VY.[13]
        {
            ctx.reg.V[X] = ctx.reg.V[Y];
        }break;
        case 0x1: // 8XY1	BitOp	Vx |= Vy	Sets VX to VX or VY. (bitwise OR operation).[13]
        {
            ctx.reg.V[X] |= ctx.reg.V[Y];
        }break;
        case 0x2: // 8XY2	BitOp	Vx &= Vy	Sets VX to VX and VY. (bitwise AND operation).[13]
        {
            ctx.reg.V[X] &= ctx.reg.V[Y];
        }break;
        case 0x3: // 8XY3[lower-alpha 1]	BitOp	Vx ^= Vy	Sets VX to VX xor VY.[13]
        {
            ctx.reg.V[X] ^= ctx.reg.V[Y];
        }break;
        case 0x4: // 8XY4	Math	Vx += Vy	Adds VY to VX. VF is set to 1 when there's an overflow, and to 0 when there is not.[13]
        {
            if (ctx.reg.V[X] + ctx.reg.V[Y] > 256)
                ctx.reg.V[0xF] = 1;
            else
                ctx.reg.V[0xF] = 0;
            ctx.reg.V[X] += ctx.reg.V[Y];
        }break;
        case 0x5: // 8XY5	Math	Vx -= Vy	VY is subtracted from VX. VF is set to 0 when there's an underflow, and 1 when there is not. (i.e. VF set to 1 if VX >= VY and 0 if not).[13]
        {
            if (ctx.reg.V[X] < ctx.reg.V[Y])
                ctx.reg.V[0xF] = 0;
            else
                ctx.reg.V[0xF] = 1;
            ctx.reg.V[X] -= ctx.reg.V[Y];
        }break;
        case 0x6: // 8XY6[lower-alpha 1]	BitOp	Vx >>= 1	Stores the least significant bit of VX in VF and then shifts VX to the right by 1.[lower-alpha 2][13]
        {
            ctx.reg.V[0xF] = ctx.reg.V[X] & 0x1;
            ctx.reg.V[X] = ctx.reg.V[X] >> 1;
        }break;
        case 0x7: // 8XY7[lower-alpha 1]	Math	Vx = Vy - Vx	Sets VX to VY minus VX. VF is set to 0 when there's an underflow, and 1 when there is not. (i.e. VF set to 1 if VY >= VX).[13]
        {
            if (ctx.reg.V[Y] < ctx.reg.V[X])
                ctx.reg.V[0xF] = 0;
            else
                ctx.reg.V[0xF] = 1;
            ctx.reg.V[X] = ctx.reg.V[Y] - ctx.reg.V[X];
        }break;
        case 0xE: // 8XYE[lower-alpha 1]	BitOp	Vx <<= 1	Stores the most significant bit of VX in VF and then shifts VX to the left by 1.[lower-alpha 2][13]
        {
            ctx.reg.V[0xF] = (ctx.reg.V[X] & 80) >> 7;
            ctx.reg.V[X] = ctx.reg.V[X] << 1;
        }break;

        }
        break;
    case 0x9: //9XY0(skip if VX != VY)
    {
        if (ctx.reg.V[X] != ctx.reg.V[Y]) ctx.reg.PC += 2;
    }
    break;
    case 0xA: //ANNN (set index register I)
    {
        ctx.reg.I = NNN;
    }
    break;
    case 0xB: //BNNN (jump to NNN + V0)
    {
        ctx.reg.PC = ctx.reg.V[0] + NNN;
    }
    break;
    case 0xC: //CXNN (Vx = rand() & NN)
    {
        ctx.reg.V[X] = (rand() % 256) & NN;
    }
    break;
    case 0xD: //DXYN (display/draw)
    {

        /*
        (x,y)
        ........
        ........
        ........     N
        ~
        ........
        */
        u16 cor_x = ctx.reg.V[X];
        u16 cor_y =ctx.reg.V[Y];
        u16 base_address = cor_y * PPU_SCREEN_WIDTH + cor_x;
        for (u16 row = 0; row < N; row++) {
            u8 pixel_index = mem_read(ctx.reg.I + row);
            for (u16 col = 0; col < 8 ; col ++) {
               //if( (pixel_index >> col ) & 0x1) 
                if (pixel_index & (0x80 >> col) )
                ppu_set_pixel(base_address + row * PPU_SCREEN_WIDTH + col);
            }
        }
        ctx.is_draw = true;

    }
    break;

    case 0xE: //CXNN (Vx = rand() & NN)
    {
        /*
        EX9E	KeyOp	if (key() == Vx)	Skips the next instruction if the key stored in VX is pressed (usually the next instruction is a jump to skip a code block).[13]
        EXA1	KeyOp	if (key() != Vx)	Skips the next instruction if the key stored in VX is not pressed (usually the next instruction is a jump to skip a code block).[13]
        */
    }
    break;

    case 0xF:
    {
        /*
        FX07	Timer	Vx = get_delay()	Sets VX to the value of the delay timer.[13]
        FX0A	KeyOp	Vx = get_key()	A key press is awaited, and then stored in VX (blocking operation, all instruction halted until next key event).[13]
        FX15	Timer	delay_timer(Vx)	Sets the delay timer to VX.[13]
        FX18	Sound	sound_timer(Vx)	Sets the sound timer to VX.[13]
        FX1E	MEM	I += Vx	Adds VX to I. VF is not affected.[lower-alpha 3][13]
        FX29	MEM	I = sprite_addr[Vx]	Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.[13]
        FX33	BCD
        set_BCD(Vx)
        *(I+0) = BCD(3);
        *(I+1) = BCD(2);
        *(I+2) = BCD(1);
        Stores the binary-coded decimal representation of VX, with the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.[13]
        FX55	MEM	reg_dump(Vx, &I)	Stores from V0 to VX (including VX) in memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.[lower-alpha 4][13]
        FX65	MEM	reg_load(Vx, &I)	Fills from V0 to VX (including VX) with values from memory, starting at address I. The offset from I is increased by 1 for each value read, but I itself is left unmodified.[lower-alpha 4][13]
        */
    }
    break;
    }

    return true;
}