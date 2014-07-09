#include<gb/gb.h>

#include "gameplay.h"
#include "LP1.297a.h"

#include "tiles/title.c"

static void show_title();

void main() {
    int clock01 = 0;
    
    show_title();
    waitpad(255);    
    
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

static void show_title() {
    disable_interrupts();
	DISPLAY_OFF;
	
	LCDC_REG = 0x01;
	BGP_REG = OBP0_REG = OBP1_REG = 0xE4U;
	
    set_bkg_data(0, 255, title_tiledata);
	set_bkg_tiles(0, 0, 20, 18, title_tilemap);
	
	DISPLAY_ON;
	enable_interrupts();
}
