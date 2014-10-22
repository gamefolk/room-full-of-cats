#include <gb/gb.h>
#include <string.h>

#include "text.h"

void load_font() {
    /* Align the font tiles with ASCII values. */
    set_bkg_data(20, 96, (UBYTE*)font_tiledata + 20U * 16);
}

void draw_text(UBYTE x, UBYTE y, const char *text) {
    UBYTE len = strlen((BYTE*)text);
    if (len + x > 20) {
        len = 20 - x;
    }

    set_bkg_tiles(x, y, len, 1, (UBYTE*)text);
}

void draw_text_win(UBYTE x, UBYTE y, const char *text) {
    UBYTE len = strlen((BYTE*)text);
    if (len + x > 20) {
        len = 20 - x;
    }

    set_win_tiles(x, y, len, 1, (UBYTE*)text);
}

void draw_ubyte_win(UBYTE x, UBYTE y, UBYTE number) {
    static char digit_buffer[3];

    digit_buffer[0] = (number / 100) + 48;
    digit_buffer[1] = ((number % 100) / 10) + 48;
    digit_buffer[2] = (number % 10) + 48;

    set_win_tiles(x, y, 3, 1, digit_buffer);
}
