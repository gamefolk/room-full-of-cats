#ifndef TEXT_H
#define TEXT_H

#include <gb/gb.h>

#include "tiles/font.c"

void load_font();
void draw_text(UBYTE x, UBYTE y, const char *text);
void draw_text_win(UBYTE x, UBYTE y, const char *text);
void draw_ubyte_win(UBYTE x, UBYTE y, UBYTE number);

#endif /* TEXT_H */

