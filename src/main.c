#include<gb/gb.h>

#include "gameplay.h"
#include "LP1.297a.h"

void main() {    
    int clock01 = 0;
    
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


