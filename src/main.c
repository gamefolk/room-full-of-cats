#include<gb/gb.h>

#include "gameplay.h"
#include "music/LP1.297a.c"

void main() {
    BOOLEAN should_loop = 1;
    
    // set up lemon music player
    clock01=0;
    step=0;
    patern=0;
    stopmusic();
    patern_definition();

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


