#include <gb/gb.h>

#include "title.h"
#include "tutorial.h"
#include "gameplay.h"
#include "LP1.297a.h"

int main() {
    int clock01 = 0;
	
	show_splash();
	show_title();

	show_tutorial();
	
    init_music();
    init_gameplay();

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
        do_gameplay();
        enable_interrupts();
    }
}
