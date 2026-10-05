// DOOM on TINY-32 / TINY-64: time and keys from ports (see rv32sim.c's memory map); the picture is i_video_tiny32.c's.
#include <stdint.h>

#include "doomgeneric.h"
#include "doomkeys.h"
#include "i_video.h"

#define PORT(at) (*(volatile uint32_t*)(at))
#define TICKS PORT(0xF0000004u)
#define KEYS PORT(0xF0000008u)

void DG_Init(void) {}

void DG_DrawFrame(void) {}   // (the frame is on the screen already: i_video_tiny32.c)

void DG_SleepMs(uint32_t ms) {
    uint32_t t = TICKS;
    while (TICKS - t < ms) ;
}

uint32_t DG_GetTicksMs(void) { return TICKS; }

// The keys: one bit each in KEYS (1 while held, from key parts on the board); a change is a press or a release
static const unsigned char KEYMAP[] = {
    KEY_UPARROW, KEY_DOWNARROW, KEY_LEFTARROW, KEY_RIGHTARROW, KEY_FIRE, KEY_USE, KEY_RSHIFT, KEY_RALT, KEY_ENTER,
    KEY_ESCAPE, 'y', 'n', '1', '2', '3', '4', '5', '6', '7', KEY_TAB, KEY_UPARROW, KEY_DOWNARROW, KEY_STRAFE_L,
    KEY_STRAFE_R, KEY_USE, KEY_MINUS, KEY_EQUALS};

int DG_GetKey(int* pressed, unsigned char* key) {
    static uint32_t held;
    uint32_t now = KEYS, changed = now ^ held;
    for (unsigned b = 0; b < sizeof KEYMAP; b++)
        if (changed >> b & 1) {
            held ^= 1u << b;
            *key = KEYMAP[b];
            *pressed = (int)(now >> b & 1);
            return 1;
        }
    return 0;
}

void DG_SetWindowTitle(const char* title) { (void)title; }

int main(void) {
    static char* argv[] = {"doom", "-iwad", "doom1.wad", 0};
    doomgeneric_Create(3, argv);
    for (;;) doomgeneric_Tick();
}
