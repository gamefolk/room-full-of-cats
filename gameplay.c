#include<gb/gb.h>
#include<gb/console.h>
#include<stdio.h>

void do_gameplay() {
    static const int VBLANK_LIMIT = 50;
    static int vblanks = 0;
    vblanks++;

    if (vblanks > VBLANK_LIMIT) {
        printf("sup\n");
        vblanks = 0;
    }
}
