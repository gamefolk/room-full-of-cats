#include<gb/gb.h>
#include<stdio.h>
#include<gb/console.h>

void play_music(BOOLEAN);
void do_gameplay();
void init_music();

void main() {
    BOOLEAN should_loop = 1;
    BOOLEAN note_change = 0;
    UBYTE music_vbls = 0;
    UBYTE render_vbls = 0;

    printf("Hello world\n");
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


