#include<gb/gb.h>

#include "gameplay.h"
#include "music.h"

void main() {
    BOOLEAN should_loop = TRUE;

    init_gameplay();
    init_music();

    enable_interrupts();
    while(TRUE) {
        wait_vbl_done();

        // Game loop begins here
        play_music(should_loop);
        do_gameplay();
        // Game loop ends here

    }
}


