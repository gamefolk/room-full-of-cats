#include <gb/gb.h>

#include "text.h"

void show_tutorial() {
	disable_interrupts();
    DISPLAY_OFF;

    LCDC_REG = 0x6F;
    BGP_REG = 0xE4U;

    load_font();
	draw_text(0, 0, "TUTORIAL");

    DISPLAY_ON;
    enable_interrupts();
	
	while(!(joypad() & J_START)) {}
}