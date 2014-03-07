#include "song.c"

int patern ;
int song_nbr ;
int nbr_patern = 3 ;

UWORD *data_song_ptr;

void patern_definition() {
    if(patern == 0){ data_song_ptr = data_song_0 ; }
    if(patern == 1){ data_song_ptr = data_song_1 ; }
    if(patern == 2){ data_song_ptr = data_song_2 ; }
}