#include <gb/gb.h>
#include <rand.h>
#include <stdio.h>

#include "tiles/fallcats.c"
#include "tiles/blank.c"
#include "tiles/stackcats.c"
#include "tiles/background.c"
#include "tiles/bgtiles.c"

// Rows and columns in the cat table
#define NUM_ROWS 4
#define NUM_COLS 3

// Locations of sprites in memory
#define BLANK_SPRITE_MEMORY 0x00
#define STRIPED_CAT_MEMORY  0x04
#define BLACK_CAT_MEMORY    0x08
#define FALLING_CAT_MEMORY  0x0C
#define SIAMESE_CAT_MEMORY  0x10

// Locations of tiles in memory
#define BLANK_TILE_MEMORY 0x14
#define CAT_FACE_MEMORY   0x18

// The number of tiles that makes up one sprite
#define TILES_IN_SPRITE 4

// Number of pixels that define graphics
#define GRID_START_X    32
#define GRID_START_Y    32
#define COLUMN_SPACING  8
#define ROW_SPACING     8
#define CAT_WIDTH       16
#define CAT_HEIGHT      16

// Function prototypes
void set_sprite_tile_16(UBYTE, UBYTE);
void assign_new_cat(UBYTE);
void move_sprite_16(UBYTE nb, UBYTE x, UBYTE y);

// Number of vblanks until gameplay update
const UWORD VBLANK_LIMIT = 60;

// An array to hold all of the sprite IDsa
UWORD cat_table [NUM_ROWS][NUM_COLS];

struct bucket_entry {
    UBYTE cat_mem;
    UBYTE num_cats;
}

struct bucket_entry buckets[4];

fixed seed;

BOOLEAN need_new_cats = FALSE;

void blank(UBYTE sprite_id) {
    set_sprite_tile_16(sprite_id, BLANK_SPRITE_MEMORY);
}

void init_cat_table() {
    int i, j;

    UBYTE sprite_id = 0x0;
    for(i = 0; i < NUM_ROWS; i++) {
        for(j = 0; j < NUM_COLS; j++) {
            blank(sprite_id);
            cat_table[i][j] = sprite_id;
            sprite_id += 0x2;
        }
    }
}

void assign_new_cat(UBYTE sprite_id) {
    INT8 gen = rand();

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
    UBYTE x, y;
    int i, j;

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

//    // Redraw the buckets
////    for (i = 0; i < NUM_COLS; i++) {
//        struct bucket_entry entry = buckets[i];
//    }

    // TODO: Redraw the buckets and score
}

/*
 * Shifts the rows of the cat table down by one, and assigns the bottom row to
 * the top row.
 */
void update_cats() {
    UBYTE tmp [NUM_COLS];
    int i, j;
    
    // Update the cat table
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

/*
    // Update the buckets
    for(i = 0; i < NUM_COLS; i++) {
        UBYTE fallen_cat = get_sprite_tile(tmp[i]);
        struct bucket_entry *bucket = &buckets[i];


        // Make sure that the fallen cat matches the bucket
        if (fallen_cat == BLANK_SPRITE_MEMORY) {
            // Do nothing
        } else if (fallen_cat == bucket->cat_mem) {
            if (bucket->num_cats < 3) {
                bucket->num_cats++;
            } else {
                bucket->num_cats = 0;

                // TODO: Increase score
            }
        } else {
            // Wrong cat type. Empty the bucket
            bucket->num_cats = 1;
            bucket->cat_mem = fallen_cat;
        }
    }*/
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
//    set_sprite_data(BLANK_TILE_MEMORY, sizeof(blank8) / 4, blank8);
//    set_sprite_data(CAT_FACE_MEMORY, sizeof(faces) / 4, faces);
    
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
