#include <gb/gb.h>

#include "title.h"
#include "tutorial.h"
#include "gameplay.h"
#include "LP1.297a.h"

static void clear_background();

int main() {
    int clock01 = 0;
    UBYTE game_flag;
    UBYTE* options;

	show_splash();
	show_title();

    disable_interrupts();
    DISPLAY_OFF;

    load_game();

    /* Main program loop */
    while (1) {
        HIDE_WIN;
        HIDE_SPRITES;

        clear_background();
        init_tutorial();

        DISPLAY_ON;
        enable_interrupts();

        options = do_tutorial();

        disable_interrupts();
        DISPLAY_OFF;

        clear_background();

        init_music();
        init_gameplay(options);

        SHOW_WIN;
        SHOW_SPRITES;

        DISPLAY_ON;
        enable_interrupts();

        /* Game loop */
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

            game_flag = do_gameplay();

            if (game_flag == 2) {
                break;
            } else if (game_flag == 1) {
                stopmusic();

                if (!pause_game()) {
                    break;
                }
            }

            enable_interrupts();
        }

        stopmusic();
        resetmusic();

        DISPLAY_OFF;
    }
}

static void clear_background() {
    static UWORD i;

    /* clear the tile map */
    for (i = 0; i < 1024; i++) {
        *(UWORD*)(0x9800 + i) = 0;
    }
}
