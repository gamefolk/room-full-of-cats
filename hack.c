#include <gb/gb.h>
#include "background.c"
#include "bgtiles.c"

/*const signed char tileMap[] = {
	0, 1, 2, 3, 0, 1, 2, 3
};*/

void make_background()
{
	UWORD i; // 8 bit index counter
	unsigned char buttons; //button press
	unsigned char tilesFromTo;
	tilesFromTo=0;

//	DISPLAY_OFF;
//	HIDE_BKG;
//	LCDC_REG = 0x01; //  [FF40] = 0b00000001 = display off, window tile map = 9800-9BFF, window off, BG tile data @ 8800-97FF, BG tile map @ 9800-9BFF, sprite size = 8x8, sprites off, BG on
//	BGP_REG = OBP0_REG = OBP1_REG = 0xE4U; // [FF47] = [FF48] = [FF49] = 11100100 = 3 2 1 0 = Black, Dark Grey, Light Grey, White
	
	set_bkg_data(0,10,bgtiles);
	
	//set_bkg_data(0, 4, tileData);
	
	/*for (i=0; i < 0x400; i++)
	{
		*(UWORD*)(0x9800 + i) = 10;
	}*/

	set_bkg_tiles(0,0,20,19,background);


	//set_bkg_tiles(0, 0, 20, 18, tileMap);
//	SHOW_BKG;
//	DISPLAY_ON;
}
