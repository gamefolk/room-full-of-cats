#include <gb/gb.h>

#include "text.h"

static void flash_selection();

#define FLASH_TIME 10
#define COLUMN_DEFAULT 1
#define SPEED_DEFAULT 1
#define TIME_DEFAULT 1

static const char* arrow_left = "<";
static const char* arrow_right = ">";
static const char* arrows_space = "<           >";
static const char* space_thirteen = "             ";
static const char* space_one = " ";

static const char* instructions1 = "CATCH CATS WITH";

static const char* instructions2[] = {
    "B & A.             ", "LEFT, B & A.       ", "LEFT, RIGHT, B & A."
};

static const char* instructions3 = "MATCH 5 CATS OF";
static const char* instructions4 = "SAME TYPE TO SCORE!";

static const char* option_columns = "# COLUMNS:";
static const char* option_speed = "SPEED:";
static const char* option_time = "TIME:";

static const char* press_start = "PRESS START!";

static const char* selections[] = {
    "2", "3", "4",
    "SLOW", "MEDIUM", "FAST",
    "FREE PLAY", "60", "120"
};

static const UBYTE options_columns[] = {
    2, 3, 4
};

static const UBYTE options_speed[] = {
   120, 60, 30
};

static const UBYTE options_time[] = {
    255, 60, 120
};

static UBYTE selection_index[3];
static UBYTE cur_selection;
static UBYTE flash_timer;
static UBYTE last_key;

static BOOLEAN cursor_moved;
static BOOLEAN flash_on;

UBYTE options[2];

/*
 * refreshes the UI, flashes the selection arrows and delays input (with flash_timer)
 */
static void flash_selection() {
    if (cursor_moved) {
        if (flash_on && (last_key == J_UP || last_key == J_DOWN)) {
            draw_text(1, 7 + (cur_selection * 3), arrow_left);
            draw_text(13, 7 + (cur_selection * 3), arrow_right);
        }

        switch (last_key) {
            case (J_UP):
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

        draw_text(1, 7 + (cur_selection * 3), space_thirteen);
        draw_text(2, 7 + (cur_selection * 3),
            selections[selection_index[cur_selection] + (cur_selection * 3)]);

        if (cur_selection == 0) {
            draw_text(1, 2, instructions2[selection_index[cur_selection]]);
        }

        flash_on = TRUE;
        cursor_moved = FALSE;
        flash_timer = 0;
    }

    if (flash_timer == FLASH_TIME) {
        if (flash_on) {
            draw_text(1, 7 + (cur_selection * 3), arrow_left);
            draw_text(13, 7 + (cur_selection * 3), arrow_right);
        } else {
            draw_text(1, 7 + (cur_selection * 3), space_one);
            draw_text(13, 7 + (cur_selection * 3), space_one);
        }

        flash_on = !flash_on;
        flash_timer = 0;
        last_key = 0;
    }

    flash_timer++;
}

/*
 * Initializes the state of the tutorial/ options screen and draws the initial UI
 */
void init_tutorial() {
    selection_index[0] = COLUMN_DEFAULT;
    selection_index[1] = SPEED_DEFAULT;
    selection_index[2] = TIME_DEFAULT;

    last_key = 0;
    cur_selection = 0;
    flash_timer = 0;

    flash_on = FALSE;
    cursor_moved = FALSE;

    draw_text(1, 1, instructions1);
    draw_text(1, 2, instructions2[COLUMN_DEFAULT]);
    draw_text(1, 3, instructions3);
    draw_text(1, 4, instructions4);

    draw_text(1, 6, option_columns);
    draw_text(1, 7, arrows_space);
    draw_text(2, 7, selections[COLUMN_DEFAULT]);

    draw_text(1, 9, option_speed);
    draw_text(1, 10, arrows_space);
    draw_text(2, 10, selections[SPEED_DEFAULT + 3]);

    draw_text(1, 12, option_time);
    draw_text(1, 13, arrows_space);
    draw_text(2, 13, selections[TIME_DEFAULT + 6]);

    draw_text(4, 15, press_start);
}

/*
 * Controls the UI according to user input
 * Returns the options selected as an array of UBYTEs
 */
UBYTE* do_tutorial() {
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

        flash_selection();

        wait_vbl_done();
    }

    options[0] = options_columns[selection_index[0]];
    options[1] = options_speed[selection_index[1]];
    options[2] = options_time[selection_index[2]];

    return options;
}
