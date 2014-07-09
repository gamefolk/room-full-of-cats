#include<gb/gb.h>

#include "gameplay.h"
#include "LP1.297a.h"

#include "tiles/title.c"

static void show_title(UBYTE palette);
static UBYTE palette_cycle(UBYTE palette, UBYTE target);

void main() {
    UBYTE i;
    int clock01 = 0;
    UBYTE palette = 0x00;
    
    show_title(palette);
    
    while(palette != 0xE4) {
        palette = palette_cycle(palette, 0xE4);
        BGP_REG = palette;
        for (i = 0; i < 20; i++) {
            wait_vbl_done();
        }
    }
    waitpad(255);
    
    while(palette != 0x00) {
        palette = palette_cycle(palette, 0x00);
        BGP_REG = palette;
        for (i = 0; i < 10; i++) {
            wait_vbl_done();
        }
    }
    
    init_music();
    init_gameplay();
    
    while(1) {
        // control music speed
        if (clock01>=7) {
            clock01=0;
        }
        
        wait_vbl_done();
        
        // play music
        if (!clock01) {
            music();
        }
        
        clock01+=1;
        
        disable_interrupts();

        do_gameplay();

        enable_interrupts();
    }
}

static void show_title(UBYTE palette) {
    disable_interrupts();
	DISPLAY_OFF;
	
	LCDC_REG = 0x01;
	BGP_REG = palette;
	
    set_bkg_data(0, 255, title_tiledata);
	set_bkg_tiles(0, 0, 20, 18, title_tilemap);
	
	DISPLAY_ON;
	enable_interrupts();
}

static UBYTE palette_cycle(UBYTE palette, UBYTE target) {
    UBYTE palette_high = (palette & 0xF0) >> 4;
    UBYTE palette_low = palette & 0xF;
    
    UBYTE shade3 = (palette_high & 0xC) >> 2;
    UBYTE shade2 = palette_high & 0x3;
    UBYTE shade1 = (palette_low & 0xC) >>2;
    UBYTE shade0 = palette_low & 0x3;
    
    BYTE inc = target > palette ? 0x1 : -0x1;
    
    palette_high = (shade3 != ((((target&0xF0)>>4)&0xC)>>2) ? shade3 + inc : shade3) << 2 | (shade2 != (((target&0xF0)>>4)&0x3) ? shade2 + inc : shade2);
    palette_low = (shade1 != (((target&0xF)&0xC)>>2) ? shade1 + inc : shade1) << 2 | (shade0 != ((target&0xF)&0x3) ? shade0 + inc : shade0);
    
    return palette_high << 4 | palette_low;
}
