#ifndef TEXT_H
#define TEXT_H

#include <gb/gb.h>

#include "tiles/font.c"

void load_font();
void draw_text(UBYTE x, UBYTE y, const char *text);

#endif /* TEXT_H */

