#include <gb/gb.h>

#include "gameplay.h"
#include "LP1.297a.h"
#include "text.h"

#include "tiles/splash.c"
#include "tiles/title.c"

static void wait_vblanks(UBYTE);
static void show_fullscreen(UBYTE*, UBYTE*, UBYTE);
static UBYTE palette_cycle(UBYTE, UBYTE);

const UBYTE* message_press = "PRESS";
const UBYTE* message_start = "START";

int main() {
    UWORD i;
    int clock01 = 0;
    UBYTE palette = 0x00;

    /* Fade splash screen in */

    show_fullscreen(splash_tiledata, splash_tilemap, palette);

    while (palette != 0xE4) {
        palette = palette_cycle(palette, 0xE4);
        BGP_REG = palette;
        wait_vblanks(20);
    }

    wait_vblanks(50);

    /* Fade splash screen out */

    while (palette != 0x00) {
        palette = palette_cycle(palette, 0x00);
        BGP_REG = palette;
        wait_vblanks(20);
    }

    /* Fade title screen in */

    show_fullscreen(title_tiledata, title_tilemap, palette);

    while (palette != 0xE4) {
        palette = palette_cycle(palette, 0xE4);
        BGP_REG = palette;
        wait_vblanks(20);
    }

    disable_interrupts();
    DISPLAY_OFF;

    /* copy font tiles 65 - 71 (letters A - Y) into memory, starting at 0x8E70 */
    for (i = 0; i < 0x190; i++) {
        *(UWORD*)(0x8E70 + i) = font_tiledata[i + 0x410];
    }

    DISPLAY_ON;
    enable_interrupts();

    /* Flash start text until player presses start */
    while (!(joypad() & J_START)) {
        for (i = 0; i < 5; i++) {
            if (joypad() & J_START) break;
            wait_vblanks(4);
            *(UWORD*)(0x99A4 + i) = message_press[i] + 166;
        }

        for (i = 0; i < 5; i++) {
            if (joypad() & J_START) break;
            wait_vblanks(4);
            *(UWORD*)(0x99AB + i) = message_start[i] + 166;
        }
    }

    /* Fade title screen out */
    while (palette != 0x00) {
        palette = palette_cycle(palette, 0x00);
        BGP_REG = palette;
        wait_vblanks(5);
    }

    init_music();
    init_gameplay();

    while (1) {
        /* control music speed */
        if (clock01>=7) {
            clock01=0;
        }

        wait_vbl_done();

        /* play music */
        if (!clock01) {
            music();
        }

        clock01+=1;

        disable_interrupts();
        do_gameplay();
        enable_interrupts();
    }
}

/*
 * Convenience method - waits for the given number of vblanks
 */
static void wait_vblanks(UBYTE vblanks) {
    for (; vblanks > 0; vblanks--) {
        wait_vbl_done();
    }
}

/*
 * Displays the fullscreen image with the given tile data and tile map with the
 * given palette
 */
static void show_fullscreen(UBYTE* tiles, UBYTE* map, UBYTE palette) {
    disable_interrupts();
    DISPLAY_OFF;

    LCDC_REG = 0x01;
    BGP_REG = palette;

    set_bkg_data(0, 255, tiles);
    set_bkg_tiles(0, 0, 20, 18, map);

    DISPLAY_ON;
    enable_interrupts();
}

/*
 * Returns the result of inc/decrementing each shade in the given 8bit
 * monochrome palette "toward" its cooresponding shade in the target palette.
 */
static UBYTE palette_cycle(UBYTE palette, UBYTE target) {
    UBYTE palette_high = (palette & 0xF0) >> 4;
    UBYTE palette_low = palette & 0xF;

    /* Access each 2bit shade in the palette */
    UBYTE shade3 = (palette_high & 0xC) >> 2;
    UBYTE shade2 = palette_high & 0x3;
    UBYTE shade1 = (palette_low & 0xC) >>2;
    UBYTE shade0 = palette_low & 0x3;

    BYTE inc = target > palette ? 0x1 : -0x1;

    /*
     * Compare the starting shades to the target shades and increment them
     * accordingly. Assemble shades 3 and 2 into the high nybble of a new
     * palette and 1 and 0 into the low nybble.
     */
    palette_high =
        (shade3 != ((((target & 0xF0) >> 4) & 0xC) >> 2) ? shade3 + inc : shade3) << 2 |
        (shade2 != (((target & 0xF0) >> 4) & 0x3) ? shade2 + inc : shade2);
    palette_low =
        (shade1 != (((target & 0xF) & 0xC) >> 2) ? shade1 + inc : shade1) << 2 |
        (shade0 != ((target & 0xF) & 0x3) ? shade0 + inc : shade0);

    return palette_high << 4 | palette_low;
}

