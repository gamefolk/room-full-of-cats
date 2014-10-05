#include <gb/gb.h>

#include "text.h"

static void flash_selection();

#define FLASH_TIME 10
#define COLUMN_DEFAULT 1
#define SPEED_DEFAULT 1
#define TIME_DEFAULT 1

const char* selections[] = {
	"2", "3", "4", 
	"SLOW", "MEDIUM", "FAST",
	"FREE PLAY", "60", "120"
};

UBYTE selection_index[3];
UBYTE cur_selection;
UBYTE flash_timer;
UBYTE last_key;

BOOLEAN cursor_moved;
BOOLEAN flash_on;

/*
 * VBLANK interrupt handler - refreshes the UI, flashes the selection arrows and delays input (with flash_timer)
 */
static void flash_selection() {
	if (cursor_moved) {		
		if (flash_on && (last_key == J_UP || last_key == J_DOWN)) {
			draw_text(1, 7 + (cur_selection * 3), "<           >");
			draw_text(2, 7 + (cur_selection * 3), 
				selections[selection_index[cur_selection] + (cur_selection * 3)]);
		} 
		
		switch (last_key) {
			case (J_UP) :
				if (cur_selection == 0) {
					cur_selection = 2;
				} else {
					cur_selection--;
				}
			break;
				
			case (J_DOWN):
				if (cur_selection == 2) {
					cur_selection = 0;
				} else {
					cur_selection++;
				}
			break;
			
			case (J_LEFT):
				if (selection_index[cur_selection] == 0) {
					selection_index[cur_selection] = 2;
				} else {
					selection_index[cur_selection]--;
				}
			break;
				
			case (J_RIGHT):
				if (selection_index[cur_selection] == 2) {
					selection_index[cur_selection] = 0;
				} else {
					selection_index[cur_selection]++;
				}
			break;
		}
		
		draw_text(1, 7 + (cur_selection * 3), "             ");
		draw_text(2, 7 + (cur_selection * 3), 
			selections[selection_index[cur_selection] + (cur_selection * 3)]);
		
		flash_on = TRUE;
		cursor_moved = FALSE;
		flash_timer = 0;
	}
	
	if (flash_timer == FLASH_TIME) {
		if (flash_on) {
			draw_text(1, 7 + (cur_selection * 3), "<           >");
		} else {
			draw_text(1, 7 + (cur_selection * 3), "             ");
		}
		
		draw_text(2, 7 + (cur_selection * 3), 
			selections[selection_index[cur_selection] + (cur_selection * 3)]);
		
		flash_on = !flash_on;
		flash_timer = 0;
		last_key = 0;
	}
	
	flash_timer++;
}

/*
 * Displays the UI and queues user input
 */
void show_tutorial() {
	selection_index[0] = COLUMN_DEFAULT;
	selection_index[1] = SPEED_DEFAULT;
	selection_index[2] = TIME_DEFAULT;
	
	last_key = 0;
	cur_selection = 0;
	flash_timer = 0;
	
	flash_on = FALSE;
	cursor_moved = FALSE;
	
	disable_interrupts();
    DISPLAY_OFF;

    LCDC_REG = 0x6F;
    BGP_REG = 0xE4U;

    load_font();
	
	draw_text(1, 1, "PLAY WITH");
	draw_text(1, 2, "LEFT, RIGHT, A & B.");
	draw_text(1, 3, "MATCH 5 CATS OF");
	draw_text(1, 4, "SAME TYPE TO SCORE!");

	draw_text(1, 6, "# COLUMNS:");
	draw_text(1, 7, "<           >");
	draw_text(2, 7, selections[COLUMN_DEFAULT]);
	
	draw_text(1, 9, "SPEED:");
	draw_text(1, 10, "<           >");
	draw_text(2, 10, selections[SPEED_DEFAULT + 3]);
	
	draw_text(1, 12, "TIME:");
	draw_text(1, 13, "<           >");
	draw_text(2, 13, selections[TIME_DEFAULT + 6]);
	
	draw_text(4, 15, "PRESS START!");
	
    DISPLAY_ON;
	
	/* set the VBLANK interrupt handler  */
	add_VBL(flash_selection);
	
    enable_interrupts();
	
	while(!(joypad() & J_START)) {
		if (last_key != J_DOWN && (joypad() & J_DOWN)) {
			last_key = J_DOWN;
			cursor_moved = TRUE;
		}
		else if (last_key != J_UP && (joypad() & J_UP)) {
			last_key = J_UP;
			cursor_moved = TRUE;
		}
		else if (last_key != J_RIGHT && (joypad() & J_RIGHT)) {	
			last_key = J_RIGHT;
			cursor_moved = TRUE;
		}
		else if (last_key != J_LEFT && (joypad() & J_LEFT)) {
			last_key = J_LEFT;
			cursor_moved = TRUE;
		}
	}
}
