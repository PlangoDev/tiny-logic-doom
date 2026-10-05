// DOOM's two hottest loops (two thirds of a frame), written for an in-order RISC-V: the texture and the light table
// kept in registers (DOOM's own loops read both from memory again for every dot, since a byte store could change
// them), four dots a turn, and a column's texture row as the top 7 bits of a number (one shift a dot instead of two).
// Same dots as DOOM's own (checked frame for frame: rv32sim --steady). Everything else in r_draw.c is DOOM's own.
#define R_DrawColumn R_DrawColumn_doom
#define R_DrawSpan R_DrawSpan_doom
#include "r_draw.c"
#undef R_DrawColumn
#undef R_DrawSpan

void R_DrawColumn(void) {
    int count = dc_yh - dc_yl + 1;
    if (count <= 0) return;
#ifdef RANGECHECK
    if ((unsigned)dc_x >= SCREENWIDTH || dc_yl < 0 || dc_yh >= SCREENHEIGHT) I_Error("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif
    byte* dest = ylookup[dc_yl] + columnofs[dc_x];
    const byte* const source = dc_source;
    const lighttable_t* const colormap = dc_colormap;
    // ((frac >> 16) & 127) is bits 16-22 of frac: shifted up 9, they're the top 7 and wrap the same way
    unsigned frac = (unsigned)(dc_texturemid + (dc_yl - centery) * dc_iscale) << 9;
    const unsigned step = (unsigned)dc_iscale << 9;
    for (; count >= 4; count -= 4, dest += 4 * SCREENWIDTH) {
        dest[0] = colormap[source[frac >> 25]];
        frac += step;
        dest[SCREENWIDTH] = colormap[source[frac >> 25]];
        frac += step;
        dest[2 * SCREENWIDTH] = colormap[source[frac >> 25]];
        frac += step;
        dest[3 * SCREENWIDTH] = colormap[source[frac >> 25]];
        frac += step;
    }
    for (; count > 0; count--, dest += SCREENWIDTH, frac += step) *dest = colormap[source[frac >> 25]];
}

void R_DrawSpan(void) {
#ifdef RANGECHECK
    if (ds_x2 < ds_x1 || ds_x1 < 0 || ds_x2 >= SCREENWIDTH || (unsigned)ds_y > SCREENHEIGHT) I_Error("R_DrawSpan: %i to %i at %i", ds_x1, ds_x2, ds_y);
#endif
    // as DOOM packs it: x in the top 16 bits, y in the bottom 16, each 6 bits whole and 10 fraction
    unsigned position = ((ds_xfrac << 10) & 0xffff0000) | ((ds_yfrac >> 6) & 0x0000ffff);
    const unsigned step = ((ds_xstep << 10) & 0xffff0000) | ((ds_ystep >> 6) & 0x0000ffff);
    byte* dest = ylookup[ds_y] + columnofs[ds_x1];
    const byte* const source = ds_source;
    const lighttable_t* const colormap = ds_colormap;
    int count = ds_x2 - ds_x1 + 1;
#define SPOT(p) ((((p) >> 4) & 0x0fc0) | ((p) >> 26))
    for (; count >= 4; count -= 4, dest += 4) {
        dest[0] = colormap[source[SPOT(position)]];
        position += step;
        dest[1] = colormap[source[SPOT(position)]];
        position += step;
        dest[2] = colormap[source[SPOT(position)]];
        position += step;
        dest[3] = colormap[source[SPOT(position)]];
        position += step;
    }
    for (; count > 0; count--, position += step) *dest++ = colormap[source[SPOT(position)]];
#undef SPOT
}
