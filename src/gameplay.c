#include <gb/gb.h>
#include <rand.h>

#include "tiles/fallcats.c"
#include "tiles/blank.c"
#include "tiles/stackcats.c"
#include "tiles/background.c"
#include "tiles/bgtiles.c"

// IDs to identify which cat is falling
const UBYTE STRIPED_CAT_ID = 0x04;
const UBYTE BLACK_CAT_ID   = 0x08;
const UBYTE FALLING_CAT_ID = 0x0C;
const UBYTE SIAMESE_CAT_ID = 0x10;

const UWORD COLUMN_SIZE  = 0x04; // cats per column
const UWORD VBLANK_LIMIT = 60;  // number of vblanks until gameplay update

UWORD colX, colY, sprID, tileID;
UWORD buckets[4]; 
UWORD score[4];

fixed seed;

void clearTiles() {
    UWORD i;
	for (i = 0; i < 0x400; i++) {
		*(UWORD*)(0x9800 + i) = 0x00;
	}
}

void makeColumn(UWORD colNum) {
	UWORD cur, y, x;
	
	cur = 0xC000;
	cur += colNum * (COLUMN_SIZE * 0x08);
		
	for (y = 0x20; y < (0x20 + ((UBYTE)COLUMN_SIZE * 0x10)); y += 0x14) {
		x = colX;
		
		*(UBYTE*)(cur) = y;
		*(UBYTE*)(cur + 0x01) = x;
		*(UBYTE*)(cur + 0x02) = 0x00;
		*(UBYTE*)(cur + 0x03) = 0x00;
	
		x += 0x08;
	
		*(UBYTE*)(cur + 0x04) = y;
		*(UBYTE*)(cur + 0x05) = x;
		*(UBYTE*)(cur + 0x06) = 0x02;
		*(UBYTE*)(cur + 0x07) = 0x00;
		
		cur += 0x08;
	}
}

UWORD setColumn(UWORD colNum) {
	UWORD cur, last;

	cur = 0xC002;
	cur += colNum * (COLUMN_SIZE * 0x08);
	cur += colY * 0x08;
	
	last = *(UBYTE*)(cur);
	
	*(UBYTE*)(cur) = sprID;
	*(UBYTE*)(cur + 0x04) = sprID + 0x02;
	
	return last;
}

/*void setTarget()
{
	UWORD cur;
	
	*(UBYTE*)(lastTarget) = 0x00;
	*(UBYTE*)(lastTarget + 0x04) = 0x00;

	cur = 0xC003;
	cur += colNum * (COLUMNSIZE * 0x08);
	cur += colY * 0x08;
		
	*(UBYTE*)(cur) = 0x20;
	*(UBYTE*)(cur + 0x04) = 0x20;
	
	lastTarget = cur;
}*/

void setBuckets(UWORD colNum) {
    if (sprID == STRIPED_CAT_ID) {
        if (buckets[colNum] == 0x03) {
            score[colNum] = 0x03;
        } else {
            buckets[colNum] = 0x03;
            score[colNum] = 0;
        }
    } else if (sprID == BLACK_CAT_ID) {
        if (buckets[colNum] == 0x04) {
            score[colNum] = 0x04;
        } else {
            buckets[colNum] = 0x04;
            score[colNum] = 0;
        }
    } else if (sprID == FALLING_CAT_ID) {
			if (buckets[colNum] == 0x02) {
				score[colNum] = 0x02;
			} else {
				buckets[colNum] = 0x02;
				score[colNum] = 0;
			}
    } else if (sprID == SIAMESE_CAT_ID) {
        if (buckets[colNum] == 0x01) {
            score[colNum] = 0x01;
        } else {
            buckets[colNum] = 0x01;
            score[colNum] = 0;
        }
	}
}

void moveRow() {	
	UWORD startY;
    UWORD colNum;	
	
	startY = colY;
	for (colNum = 0x00; colNum < 0x03; colNum++) {
		sprID = 0x00;
		sprID = setColumn(colNum);
		
		if (colY < 0x03) {
			colY += 0x01;
			setColumn(colNum);
		} else {
			setBuckets(colNum);
		}
		
		colY = startY;
	}
}

void moveRows() {
	colY = 0x03;
	moveRow();
	colY = 0x02;
	moveRow();
	colY = 0x01;
	moveRow();
	colY = 0x00;
	moveRow();
}

UWORD pickCat() {
	UINT8 gen = rand();
	
	if (gen & 1)
		return STRIPED_CAT_ID;
    else if (gen & 4)
		return BLACK_CAT_ID;
	else if (gen & 16)
		return FALLING_CAT_ID;
    else
		return SIAMESE_CAT_ID;
}

void startRow() {	
    UWORD colNum;
    const UWORD NUM_COLS = 0x03;
    for(colNum = 0; colNum < NUM_COLS; colNum++) {
        colY = 0x00;
        sprID = pickCat();
        setColumn(colNum);
    }
}

void load_sprite(UWORD mem_offset, UBYTE* sprite) {
    const UWORD sprite_offset = 0x40; 
    UWORD i;
   
   for(i = 0; i < sprite_offset; i++) {
       *(UWORD*)(mem_offset + i) = sprite[i];
   }
}

void init_gameplay() {
	UWORD i;
	
	DISPLAY_OFF;
	
	LCDC_REG = 0x01; 
	BGP_REG = OBP0_REG = OBP1_REG = 0xE4U; 
	
	SPRITES_8x16;

    // Initialize random number generator with contents of DIV_REG    
	seed.b.l = DIV_REG;
	seed.b.h = DIV_REG;
	initrand(seed.w);

    // make_background();
	
	// load sprite tiles
    load_sprite(0x8000, blank16);
    load_sprite(0x8040, cat0);
    load_sprite(0x8080, cat1);
    load_sprite(0x80C0, cat2);
    load_sprite(0x8100, cat3);
	
	// load tile tiles
	for(i = 0; i < 0x10; i++)
		*(UWORD*)(0x9000+i) = blank8[i];
	
	for(i = 0; i < 0x40; i++)
		*(UWORD*)(0x9010+i) = faces[i];
	
	// clear background tiles
	clearTiles();
	
    // background code
    //set_bkg_data(0,10,bgtiles);
	
	//set_bkg_data(0, 4, tileData);
	
	/*for (i=0; i < 0x400; i++) {
		*(UWORD*)(0x9800 + i) = 10;
	}*/

	//set_bkg_tiles(0,0,20,19,background);
	
	// set up columns
	
	colX = 0x2B;
	
	for (i = 0; i < 3; i++) {
		makeColumn(i);
		colX += 0x18;
	}
	
	startRow();
	
	SHOW_SPRITES;
	DISPLAY_ON;
	
	colY = 0x00;	
}

void do_gameplay() {
    static UBYTE vblanks = 0;
    UBYTE buttons;
    UWORD i;
    UWORD colNum = 0x00;

    vblanks++;

    buttons = joypad();
    switch(buttons) {
        case(J_LEFT):
            colNum = 0x00;
            colY = 0x02;
            sprID = 0x00;

            setColumn(colNum);
        break;
        
        case(J_DOWN):
            colNum = 0x01;
            colY = 0x02;
            sprID = 0x00;

            setColumn(colNum);
        break;
        
        case(J_RIGHT):
            colNum = 0x02;
            colY = 0x02;
            sprID = 0x00;

            setColumn(colNum);
        break;
    }

    if (vblanks > VBLANK_LIMIT) {
        vblanks = 0;

        for (i = 0; i < 4; i++) {
            UWORD columnOffset;
			columnOffset = 3 * i ;
			*(UWORD*)(0x9800 + 0x1C0 + 0x04 + columnOffset) = buckets[i];
			*(UWORD*)(0x9801 + 0x1C0 + 0x04 + columnOffset) = score[i];
		}
		
		/*for (i = 0; i < score[0]; i++) {
			*(UWORD*)(0x9800 + 0x1C0 + 0x04 + i) = buckets[0];
		}*/
		
		/*colNum = targetX;
		colY = targetY;
		setTarget();*/
		
		moveRows();
		startRow();
    }
}
