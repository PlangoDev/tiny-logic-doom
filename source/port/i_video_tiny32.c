// DOOM's video layer for TINY-32 / TINY-64, instead of doomgeneric's i_video.c (which draws into a buffer, copies it
// into another, then converts that a pixel at a time: ~400k instructions a frame, more than a quarter of the work).
//   make PAL=1: a palette screen. DOOM draws straight into the screen's memory (a palette index a dot) and its palette
//               goes to the screen's 256 colours (a word 0x00RRGGBB each) when it changes: a frame costs nothing.
//   make      : an RRRGGGBB screen. DOOM draws in RAM, and each frame is turned into those colours straight onto the
//               screen (a word read, four bytes written).
#include <stdint.h>
#include <string.h>

#include "d_event.h"
#include "doomgeneric.h"
#include "i_video.h"
#include "tables.h"
#include "v_video.h"
#include "z_zone.h"

#define PORT(at) (*(volatile uint32_t*)(at))
#define FRAME PORT(0xF000000Cu)
#define PALETTE ((volatile uint32_t*)0xF0001000u)
#define SCREEN ((byte*)0xF0010000u)

byte* I_VideoBuffer = NULL;
boolean screensaver_mode = false, screenvisible, palette_changed;
struct color colors[256];
float mouse_acceleration = 2.0;
int mouse_threshold = 10, usegamma = 0, usemouse = 0, fb_scaling = 1;

#ifndef TINY32_PALETTE
static uint8_t to332[256];   // DOOM's palette, each colour as the nearest RRRGGGBB
#endif

void I_GetEvent(void);
void I_InitInput(void);

void I_InitGraphics(void) {
#ifdef TINY32_PALETTE
    I_VideoBuffer = SCREEN;
#else
    I_VideoBuffer = (byte*)Z_Malloc(SCREENWIDTH * SCREENHEIGHT, PU_STATIC, NULL);
#endif
    screenvisible = true;
    I_InitInput();
}

void I_SetPalette(byte* palette) {
    for (int i = 0; i < 256; i++) {
        const uint32_t r = gammatable[usegamma][*palette++], g = gammatable[usegamma][*palette++], b = gammatable[usegamma][*palette++];
        colors[i].r = (byte)r, colors[i].g = (byte)g, colors[i].b = (byte)b, colors[i].a = 0;
#ifdef TINY32_PALETTE
        PALETTE[i] = r << 16 | g << 8 | b;
#else
        to332[i] = (uint8_t)((r + 16) / 36 << 5 | (g + 16) / 36 << 2 | (b + 32) / 85);
#endif
    }
}

void I_FinishUpdate(void) {
#ifndef TINY32_PALETTE
    const uint32_t* in = (const uint32_t*)I_VideoBuffer;
    volatile uint8_t* out = (volatile uint8_t*)SCREEN;   // (the screen takes a byte a write)
    for (int i = 0; i < SCREENWIDTH * SCREENHEIGHT / 4; i++, out += 4) {
        const uint32_t w = in[i];
        out[0] = to332[w & 255], out[1] = to332[(w >> 8) & 255], out[2] = to332[(w >> 16) & 255], out[3] = to332[w >> 24];
    }
#endif
    FRAME = 1;
}

void I_ReadScreen(byte* scr) { memcpy(scr, I_VideoBuffer, SCREENWIDTH * SCREENHEIGHT); }

int I_GetPaletteIndex(int r, int g, int b) {   // (the nearest of DOOM's colours)
    int best = 0, best_diff = 1 << 30;
    for (int i = 0; i < 256 && best_diff; i++) {
        const int dr = r - colors[i].r, dg = g - colors[i].g, db = b - colors[i].b, diff = dr * dr + dg * dg + db * db;
        if (diff < best_diff) best = i, best_diff = diff;
    }
    return best;
}

void I_ShutdownGraphics(void) {}
void I_StartFrame(void) {}
void I_StartTic(void) { I_GetEvent(); }
void I_UpdateNoBlit(void) {}
void I_BeginRead(void) {}
void I_EndRead(void) {}
void I_SetWindowTitle(char* title) { DG_SetWindowTitle(title); }
void I_GraphicsCheckCommandLine(void) {}
void I_SetGrabMouseCallback(grabmouse_callback_t func) { (void)func; }
void I_EnableLoadingDisk(void) {}
void I_BindVideoVariables(void) {}
void I_DisplayFPSDots(boolean dots_on) { (void)dots_on; }
void I_CheckIsScreensaver(void) {}
