/*
	File for the music definitions to use with the Lemon Player

	created with mod2gbl 0.6

	Whoops, in fact you will need to create this file with the part of the song data
	created with mod2gbl.
	Just follow this exemple

	on 26 sept 99

	lemon@urbanet.ch

*/

int patern ;
int nbr_patern ;
int song_nbr ;

UWORD *data_song_ptr;

extern UWORD data_song_fullos0[];
extern UWORD data_song_fullos1[];
extern UWORD data_song_fullos2[];
extern UWORD data_song_fullos3[];
extern UWORD data_song_fullos4[];
extern UWORD data_song_fullos5[];
extern UWORD data_song_fullos6[];
extern UWORD data_song_fullos7[];
extern UWORD data_song_fullos8[];

void patern_definition()
{
    if(song_nbr == 0){
         if(patern == 0){ data_song_ptr = data_song_fullos0 ; }
         if(patern == 1){ data_song_ptr = data_song_fullos1 ; }
         if(patern == 2){ data_song_ptr = data_song_fullos2 ; }
         if(patern == 3){ data_song_ptr = data_song_fullos3 ; }
         if(patern == 4){ data_song_ptr = data_song_fullos4 ; }
         if(patern == 5){ data_song_ptr = data_song_fullos5 ; }
         if(patern == 6){ data_song_ptr = data_song_fullos6 ; }
         if(patern == 7){ data_song_ptr = data_song_fullos7 ; }
         if(patern == 8){ data_song_ptr = data_song_fullos8 ; }
    }
}

