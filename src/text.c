#include <gb/gb.h>
#include <string.h>

#include "text.h"

void load_font() {
    /* Align the font tiles with ASCII values. */
    set_bkg_data(20, 96, (UBYTE*)font_tiledata + 20U * 16);
}

void draw_text(UBYTE x, UBYTE y, const char *text) {
    int len = strlen((BYTE*)text);
    if (len + x > 20) {
        len = 20;
    }

    set_bkg_tiles(x, y, len, 1, (UBYTE*)text);
}

