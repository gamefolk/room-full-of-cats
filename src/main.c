#include<gb/gb.h>
#include<stdio.h>
#include<gb/console.h>

#include "include/gameplay.h"
#include "include/music.h"

void main() {
    BOOLEAN should_loop = 1;

    init_gameplay();
    init_music();
    while(1) {
        wait_vbl_done();
        disable_interrupts();

        // Game loop begins here
        play_music(should_loop);
        do_gameplay();
        // Game loop ends here

        enable_interrupts();
    }
}


