#include "song.c"

int patern ;
int song_nbr ;
int nbr_patern = 5 ;

UWORD *data_song_ptr;

void patern_definition()
{
         if(patern == 0){ data_song_ptr = data_song_0 ; }
         if(patern == 1){ data_song_ptr = data_song_1 ; }
         if(patern == 2){ data_song_ptr = data_song_2 ; }
         if(patern == 3){ data_song_ptr = data_song_3 ; }
         if(patern == 4){ data_song_ptr = data_song_4 ; }
}