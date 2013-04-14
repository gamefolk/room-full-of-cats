#include <gb/gb.h>
#include <rand.h>

#include "tiles/fallcats.c"
#include "tiles/blank.c"
#include "tiles/stackcats.c"
#include "tiles/background.c"
#include "tiles/bgtiles.c"

#include "music.h"

#define COLUMNSIZE 0x04 // cats per column

UWORD colNum, colX, colY, sprID, tileID, temp, i;
UWORD buckets[4]; 
UWORD score[4];
unsigned char buttons;
INT8 gen;
fixed seed;

//void make_background();

void clearTiles()
{
	for (i=0; i < 0x400; i++)
	{
		*(UWORD*)(0x9800 + i) = 0x00;
	}
}

void makeColumn()
{
	UWORD cur, y, x;
	
	cur = 0xC000;
	cur += colNum * (COLUMNSIZE * 0x08);
		
	for (y = 0x20; y < (0x20 + (COLUMNSIZE * 0x10)); y += 0x14)
	{
		x = colX;
		
		*(unsigned char*)(cur) = y;
		*(unsigned char*)(cur + 0x01) = x;
		*(unsigned char*)(cur + 0x02) = 0x00;
		*(unsigned char*)(cur + 0x03) = 0x00;
	
		x += 0x08;
	
		*(unsigned char*)(cur + 0x04) = y;
		*(unsigned char*)(cur + 0x05) = x;
		*(unsigned char*)(cur + 0x06) = 0x02;
		*(unsigned char*)(cur + 0x07) = 0x00;
		
		cur += 0x08;
	}
}

UWORD setColumn()
{
	UWORD cur, last;

	cur = 0xC002;
	cur += colNum * (COLUMNSIZE * 0x08);
	cur += colY * 0x08;
	
	last = *(unsigned char*)(cur);
	
	*(unsigned char*)(cur) = sprID;
	*(unsigned char*)(cur + 0x04) = sprID + 0x02;
	
	return last;
}

/*void setTarget()
{
	UWORD cur;
	
	*(unsigned char*)(lastTarget) = 0x00;
	*(unsigned char*)(lastTarget + 0x04) = 0x00;

	cur = 0xC003;
	cur += colNum * (COLUMNSIZE * 0x08);
	cur += colY * 0x08;
		
	*(unsigned char*)(cur) = 0x20;
	*(unsigned char*)(cur + 0x04) = 0x20;
	
	lastTarget = cur;
}*/

void setBuckets()
{
	switch (sprID)
	{
		case(0x04):
			if (buckets[colNum] == 0x03)
			{
				score[colNum] = 0x03;
			}
			else
			{
				buckets[colNum] = 0x03;
				score[colNum] = 0;
			}
		break;
				
		case(0x08):
			if (buckets[colNum] == 0x04)
			{
				score[colNum] = 0x04;
			}
			else
			{
				buckets[colNum] = 0x04;
				score[colNum] = 0;
			}
		break;
				
		case(0x0C):
			if (buckets[colNum] == 0x02)
			{
				score[colNum] = 0x02;
			}
			else
			{
				buckets[colNum] = 0x02;
				score[colNum] = 0;
			}
		break;
				
		case(0x10):
			if (buckets[colNum] == 0x01)
			{
				score[colNum] = 0x01;
			}
			else
			{
				buckets[colNum] = 0x01;
				score[colNum] = 0;
			}
		break;
	}
}

void moveRow()
{		
	UWORD startY;
	
	startY = colY;
	
	for (colNum = 0x00; colNum < 0x03; colNum++)
	{
		sprID = 0x00;
		sprID = setColumn();
		
		if (colY < 0x03)
		{
			colY += 0x01;
			setColumn();
		}
		else
		{
			setBuckets();
		}
		
		colY = startY;
	}
}

void moveRows()
{
	colY = 0x03;
	moveRow();
	colY = 0x02;
	moveRow();
	colY = 0x01;
	moveRow();
	colY = 0x00;
	moveRow();
}

UWORD pickCat()
{
	gen = rand();
	
	if (gen < -63)
		return 0x04;
	if (gen < 0)
		return 0x08;
	if (gen < 63)
		return 0x0C;
	if (gen < 127)
		return 0x10;
}

void startRow()
{	
	colNum = 0x00;
	colY = 0x00;
	sprID = pickCat();
	
	setColumn();
	
	colNum = 0x01;
	colY = 0x00;
	sprID = pickCat();
	
	setColumn();
	
	colNum = 0x02;
	colY = 0x00;
	sprID = pickCat();
	
	setColumn();
	
	/*colNum = 0x03;
	colY = 0x00;
	sprID = pickCat();
	
	setColumn();*/
}

void main()
{
	UWORD i;
    UBYTE vblanks = 0;
	const UWORD VBLANK_LIMIT = 60;
	
	DISPLAY_OFF;
	
	LCDC_REG = 0x01; 
	BGP_REG = OBP0_REG = OBP1_REG = 0xE4U; 
	
	SPRITES_8x16;
	
	seed.b.l = DIV_REG;
	seed.b.h = DIV_REG;
	initrand(seed.w);

//    make_background();
	
	// load sprite tiles
	
	for (i = 0; i < 0x40; i++)
		*(UWORD*)(0x8000+i) = blank16[i];
	
	for (i = 0; i < 0x40; i++)
		*(UWORD*)(0x8040+i) = cat0[i];
	
	for (i = 0; i < 0x40; i++)
		*(UWORD*)(0x8080+i) = cat1[i];
	
	for (i = 0; i < 0x40; i++)
		*(UWORD*)(0x80C0+i) = cat2[i];
	
	for (i = 0; i < 0x40; i++)
		*(UWORD*)(0x8100+i) = cat3[i];
	
	// load tile tiles
	
	for(i=0; i < 0x10; i++)
		*(UWORD*)(0x9000+i) = blank8[i];
	
	for(i=0; i < 0x40; i++)
		*(UWORD*)(0x9010+i) = faces[i];
	
	// clear background tiles
	
	clearTiles();
	
   // tim shit 
    //set_bkg_data(0,10,bgtiles);
	
	//set_bkg_data(0, 4, tileData);
	
	/*for (i=0; i < 0x400; i++)
	{
		*(UWORD*)(0x9800 + i) = 10;
	}*/

	//set_bkg_tiles(0,0,20,19,background);
	
	// set up columns
	
	colNum = 0x00;
	colX = 0x2B;
	
	for (i = 0; i < 3; i++)
	{
		makeColumn();
		colNum++;
		colX += 0x18;
	}
	
	startRow();
	
	SHOW_SPRITES;
	DISPLAY_ON;
	
	colY = 0x00;	
    init_music();
	while(1)
	{
		wait_vbl_done();
        vblanks++;
		
		//clearTiles();
		
        play_music(1);	
		
        buttons = joypad();
		switch(buttons)
		{
			case(J_LEFT):
				colNum = 0x00;
				colY = 0x02;
				sprID = 0x00;
	
				setColumn();
			break;
			
			case(J_DOWN):
				colNum = 0x01;
				colY = 0x02;
				sprID = 0x00;
	
				setColumn();
			break;
			
			case(J_RIGHT):
				colNum = 0x02;
				colY = 0x02;
				sprID = 0x00;
	
				setColumn();
			break;
		}


    if (vblanks > VBLANK_LIMIT) {
        vblanks = 0;
        for (i = 0; i < 4; i++)
		{
			temp = i + i + i;
			*(UWORD*)(0x9800 + 0x1C0 + 0x04 + temp) = buckets[i];
			*(UWORD*)(0x9801 + 0x1C0 + 0x04 + temp) = score[i];
		}
		
		/*for (i = 0; i < score[0]; i++)
		{
			*(UWORD*)(0x9800 + 0x1C0 + 0x04 + i) = buckets[0];
		}*/
		
//0
		
		//disable_interrupts();
		
		//delay(1000);
		
		
		
		/*colNum = targetX;
		colY = targetY;
		setTarget();*/
		
		moveRows();
		
		startRow();
		
    }
		
		//enable_interrupts();
	}
	
}
