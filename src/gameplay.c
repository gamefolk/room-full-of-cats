#include <gb/gb.h>
#include <rand.h>
#include <string.h>
#include <stdio.h>

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

const UWORD VBLANK_LIMIT = 60;      // number of vblanks until gameplay update

// Number of rows of cats
#define NUM_ROWS 0x04

// Number of columns of cats
#define NUM_COLS 0x03

#define NUM_CATS 0x04

// Locations of sprites in memory
#define BLANK_SPRITE_MEMORY 0x00
#define STRIPED_CAT_MEMORY  0x04
#define BLACK_CAT_MEMORY    0x08
#define FALLING_CAT_MEMORY  0x0C
#define SIAMESE_CAT_MEMORY  0x10

// Locations of tiles in memory
#define BLANK_TILE_MEMORY 0x14
#define CAT_FACE_MEMORY   0x18

/*
 * Numbers to identify sprite locations. Must be multiples of 2 to facilitate
 * 16x16 sprites
 */
#define BLANK_SPRITE_ID 0
#define STRIPED_CAT_ID  2
#define BLACK_CAT_ID    4
#define FALLING_CAT_ID  6
#define SIAMESE_CAT_ID  8

#define TILES_IN_SPRITE 4

#define GRID_START_X    32
#define GRID_START_Y    32
#define COLUMN_SPACING  8
#define ROW_SPACING     8
#define CAT_WIDTH       16
#define CAT_HEIGHT      16

UBYTE striped_id, black_id, falling_id, siamese_id;
UBYTE blank16_id, blank8_id, faces_id;

UBYTE cat_ids [NUM_CATS];

// An array to hold all of the sprite IDsa
UWORD cat_table [NUM_ROWS][NUM_COLS];

UWORD tileID;
UWORD buckets[4];
UWORD score[4];

fixed seed;

BOOLEAN need_new_cats = FALSE;

// Function prototypes
void set_sprite_tile_16(UBYTE, UBYTE);
void assign_new_cat(UBYTE);
void move_sprite_16(UBYTE nb, UBYTE x, UBYTE y);

void blank(UBYTE sprite_id) {
    set_sprite_tile_16(sprite_id, BLANK_SPRITE_MEMORY);
}

void init_cat_table() {
    UBYTE i, j;

    UBYTE sprite_id = 0;
    for(i = 0; i < NUM_ROWS; i++) {
        for(j = 0; j < NUM_COLS; j++) {
            blank(sprite_id);
            cat_table[i][j] = sprite_id;
            sprite_id += 2;
        }
    }
}

void clearTiles() {
    UWORD i;
    for (i = 0; i < 0x400; i++) {
        *(UWORD*)(0x9800 + i) = 0x00;
    }
}

void assign_new_cat(UBYTE sprite_id) {
    UINT8 gen = rand();

    // Choose a cat at random.
    if (gen & 1) {
        set_sprite_tile_16(sprite_id, STRIPED_CAT_MEMORY);
    } else if (gen & 4) {
        set_sprite_tile_16(sprite_id, BLACK_CAT_MEMORY);
    } else if (gen & 16) {
        set_sprite_tile_16(sprite_id, FALLING_CAT_MEMORY);
    } else {
        set_sprite_tile_16(sprite_id, SIAMESE_CAT_MEMORY);
    }
}

void render() {
    UBYTE i, j, x, y;

    disable_interrupts();

    /*
     * Give the sprites at the top of the cat table a new sprite if they've
     * been updated recently
     */
    if(need_new_cats) {
        for (i = 0; i < NUM_COLS; i++) {
            assign_new_cat(cat_table[0][i]);
        }

        need_new_cats = FALSE;
    }
   
    // Redraw every sprite in the cat table 
    for (i = 0; i < NUM_ROWS; i++) {
        y = GRID_START_Y + i * (ROW_SPACING + CAT_HEIGHT);
        for (j = 0; j < NUM_COLS; j++) {
            x = GRID_START_X + j * (COLUMN_SPACING + CAT_WIDTH);
            move_sprite_16(cat_table[i][j], x, y);
        }
    }

    // TODO: Redraw the buckets and score
}

void setBuckets(UWORD colNum, UWORD sprID) {
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

/*
 * Shifts the rows of the cat table down by one, and assigns the bottom row to
 * the top row.
 */
void update_cats() {
    UBYTE tmp [NUM_COLS], i,j;
    
    for(j = 0; j < NUM_COLS; j++) {
        tmp[j] = cat_table[NUM_ROWS - 1][j];
    }

    for (i = NUM_ROWS - 1; i > 0; i--) {
        for (j = 0; j < NUM_COLS; j++) {
            cat_table[i][j] = cat_table[i - 1][j];
        }
    }

    for(j = 0; j < NUM_COLS; j++) {
        cat_table[0][j] = tmp[j];
    }

    need_new_cats = TRUE;

    // TODO: Update buckets
}

/*
 * Wrapper to treat two 8x16 sprites as a 16x16 sprite
 */
void set_sprite_tile_16(UBYTE nb, UBYTE tile) {
    const UBYTE TILE_OFFSET = 2;
    set_sprite_tile(nb, tile);
    set_sprite_tile(nb + 1, tile + TILE_OFFSET);
}

/*
 * Wrapper to treat two 8x16 sprites as a 16x16 sprite
 */
void move_sprite_16(UBYTE nb, UBYTE x, UBYTE y) {
    const UBYTE X_OFFSET = 0x08;
    move_sprite(nb, x, y);
    move_sprite(nb + 1, x + X_OFFSET, y);
}

/*
 * Helper function to assist in loading sprites. Returns the sprite id
 * after returning the sprite
 */
UBYTE load_sprite(UBYTE vram_location, size_t sprite_size, UBYTE* tiles) {
    const UBYTE TILE_OFFSET = 2;
    const UBYTE BYTES_IN_TILE = 16;
    static UBYTE new_id = 0;

    UBYTE sprite_id, num_tiles;
    
    num_tiles = sprite_size / 16;
    set_sprite_data(vram_location, num_tiles, tiles);
    set_sprite_tile_16(new_id, vram_location);
    sprite_id = new_id;
    new_id += TILE_OFFSET;
    return sprite_id;
}


void init_gameplay() {
    DISPLAY_OFF;
    
    LCDC_REG = 0x01;
    BGP_REG = OBP0_REG = OBP1_REG = 0xE4U;
    
    SPRITES_8x16;

    // Initialize random number generator with contents of DIV_REG
    seed.b.l = DIV_REG;
    seed.b.h = DIV_REG;
    initrand(seed.w);
    
    // TODO: Load background into VRAM
   
    // Load sprites into VRAM
    set_sprite_data(BLANK_SPRITE_MEMORY, sizeof(blank16) / TILES_IN_SPRITE, blank16);
    set_sprite_data(STRIPED_CAT_MEMORY, sizeof(cat0) / TILES_IN_SPRITE, cat0);
    set_sprite_data(BLACK_CAT_MEMORY, sizeof(cat1) / TILES_IN_SPRITE, cat1);
    set_sprite_data(FALLING_CAT_MEMORY, sizeof(cat2) / TILES_IN_SPRITE, cat2);
    set_sprite_data(SIAMESE_CAT_MEMORY, sizeof(cat3) / TILES_IN_SPRITE, cat3);

    // Load tiles into VRAM
    blank8_id = load_sprite(BLANK_TILE_MEMORY, sizeof(blank8), blank8);
    faces_id = load_sprite(CAT_FACE_MEMORY, sizeof(faces), faces);
    
    // Initialize all sprites and assign their ids to the table 
    init_cat_table();
    need_new_cats = TRUE;
    update_cats();

    SHOW_SPRITES;

    DISPLAY_ON;

    add_VBL(render);
}

void do_gameplay() {
    static UBYTE vblanks = 0;

    static UWORD column_to_remove = 0, colY = 0x02;
    UBYTE buttons;

    vblanks++;

    buttons = joypad();
    switch(buttons) {
        case(J_LEFT):
            column_to_remove = 0x00;
        break;
        
        case(J_DOWN):
            column_to_remove = 0x01;
        break;
        
        case(J_RIGHT):
            column_to_remove = 0x02;
        break;
    }

    if (vblanks > VBLANK_LIMIT) {
        vblanks = 0;
        update_cats();
    }
}
