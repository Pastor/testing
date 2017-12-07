#include <stdio.h>
#include "vm8080.h"
#include <SDL2/SDL.h>

SDL_Surface *screen;
SDL_Window *window;
SDL_Renderer *render;
SDL_Texture *texture;
SDL_Event ev;

u8 dip1 = 0x01;
u8 dip2 = 0x00;

u16 shift_register = 0x0000;
u16 shift_off = 0x0000;

RamBank bank[4] = {
        {
                0x0000,
                0x1FFF,
                0x0000,
                FLAG_ROM,
                NULL
        },
        {
                0x2000,
                0x0400,
                0x0000,
                FLAG_USED,
                NULL
        },
        {
                0x2400,
                0x1C00,
                0x0000,
                FLAG_USED,
                NULL
        },
        {
                0x4000,
                0xFFFF,
                0x2000,
                FLAG_MIRROR,
                NULL
        },
};

static u8 port_in(int port) {
    switch (port) {
        case 1:
            return dip1;
        case 2:
            return dip2;
        case 3:
            return (u8) ((shift_register << shift_off) >> 8);
        default:
            while (1);
    }

    return 0x00;
}

static void port_out(int port, u8 value) {
    switch (port) {
        case 2:
            shift_off = value;
            break;
        case 4:
            shift_register = (shift_register << 8) | value;
            break;
        default:
            break;
    }
}

static void update_display() {
    int ram_ptr, b;
    u32 *ptr;

    SDL_LockSurface(screen);
    ptr = screen->pixels;
    for (ram_ptr = 0; ram_ptr < 0x4000 - 0x2400; ram_ptr++) {
        for (b = 0; b < 8; b++) {
            *ptr = (u32) (((read_byte((u16) (0x2400 + ram_ptr)) >> b) & 1) ? 0xFFFFFFFF : 0x00000000);
            ptr++;
        }
    }
    SDL_UpdateTexture(texture, NULL, screen->pixels, screen->pitch);
    SDL_UnlockSurface(screen);
    SDL_RenderClear(render);
    SDL_RenderCopyEx( render, texture, NULL, NULL, -90, NULL, SDL_FLIP_NONE );
    SDL_RenderPresent(render);
}

static void update_input() {
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
            case SDL_KEYDOWN:
                switch (ev.key.keysym.sym) {
                    case SDLK_LEFT:
                        dip1 |= (1 << 5);
                        break;
                    case SDLK_RIGHT:
                        dip1 |= (1 << 6);
                        break;
                    case SDLK_c:
                        dip1 |= (1 << 0);
                        break;
                    case SDLK_x:
                        dip1 |= (1 << 2);
                        break;
                    case SDLK_z:
                        dip1 |= (1 << 4);
                        break;
                    default:
                        break;
                }
                break;
            case SDL_QUIT:
                exit(0);
                break;
            default:
                break;
        }
    }
}

int main(int argc, char *argv[]) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 r_mask = 0xff000000;
    Uint32 g_mask = 0x00ff0000;
    Uint32 b_mask = 0x0000ff00;
    Uint32 a_mask = 0x000000ff;
#else
    Uint32 r_mask = 0x000000ff;
    Uint32 g_mask = 0x0000ff00;
    Uint32 b_mask = 0x00ff0000;
    Uint32 a_mask = 0xff000000;
#endif
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Cannot initialize SDL\n");
        exit(0);
    }

    atexit(SDL_Quit);
    SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_ALLOW_HIGHDPI, &window, &render);
    SDL_SetWindowTitle(window, "vm8080");
    screen = SDL_CreateRGBSurface(SDL_SWSURFACE, 256, 224, 32, r_mask, g_mask, b_mask, a_mask);
    texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 256, 224);

    if (!game_vm8080()) {
        printf("Error while initializing the processor\n");
        exit(0);
    }

    cpu.port_in = port_in;
    cpu.port_out = port_out;

    while (!cpu.halt) {
        run_vm8080(28527);
        cause_int(8);
        update_display();
        run_vm8080(4839);
        cause_int(16);
        update_input();
        SDL_Delay(15);
    }
    SDL_DestroyRenderer(render);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(screen);
    SDL_DestroyWindow(window);
    return 1;
}

#include <windows.h>

INT WINAPI
WinMain(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPSTR lp_str, int n_show) {
    return main(0, NULL);
}
