#include <gb/gb.h>
#include <rand.h>
#include <stdio.h>

#include "tiles/fallcats.c"
#include "tiles/blank.c"
#include "tiles/stackcats.c"
#include "tiles/background.c"
#include "tiles/bgtiles.c"

// Rows and columns in the cat grid
#define NUM_ROWS 4
#define NUM_COLS 3

// Locations of sprites in memory
#define BLANK_SPRITE_MEMORY 0x00
#define STRIPED_CAT_MEMORY  0x04
#define BLACK_CAT_MEMORY    0x08
#define FALLING_CAT_MEMORY  0x0C
#define SIAMESE_CAT_MEMORY  0x10

// Locations of the bucket cats in tile memory
#define BLANK_TILE       0x01
#define STRIPED_CAT_TILE 0x06
#define BLACK_CAT_TILE   0x07
#define SIAMESE_CAT_TILE 0x04
#define FALLING_CAT_TILE 0x05

// Locations of background tiles in memory
#define CAT_FACE_MEMORY   0x04

// The number of tiles that makes up one sprite
#define TILES_IN_SPRITE 4

// Dimensions and locations of the cat grid
#define GRID_START_X   32
#define GRID_START_Y   32
#define COLUMN_SPACING 8
#define ROW_SPACING    8
#define BUCKET_START_X 32
#define BUCKET_START_Y 120
#define BUCKET_WIDTH   16
#define BUCKET_HEIGHT  16

// Dimensions of a cat sprite in pixels
#define CAT_WIDTH  16
#define CAT_HEIGHT 16

// Dimensions of a tile in pixels
#define TILE_WIDTH  8
#define TILE_HEIGHT 8

// Time until a gameplay update in vblanks
#define TIME_UNTIL_UPDATE 60

// Time until the joypad should be updated in vblanks
#define TIME_UNTIL_INPUT  10

// Function prototypes
void set_sprite_tile_16(UBYTE number, UBYTE tile);
void assign_new_cat(UBYTE sprite_id);
void move_sprite_16(UBYTE number, UBYTE x, UBYTE y);

// An array to hold all of the sprite IDsa
UBYTE cat_table [NUM_ROWS][NUM_COLS];

// Struct to hold bucket information
struct bucket_entry {
    UBYTE cat_mem;
    UBYTE num_cats;
}

// The buckets to catch cats in
struct bucket_entry buckets[NUM_COLS];

// The random number generator seed
fixed seed;

BOOLEAN need_new_cats = TRUE, cats_changed = TRUE;

/*
 * Convenience method to assign a blank sprite to an arbitrary sprite
 */
void blank(UBYTE sprite_id) {
    set_sprite_tile_16(sprite_id, BLANK_SPRITE_MEMORY);
}

/*
 * Assigns sprite IDs to every entry in the cat table
 */
void init_cat_table() {
    BYTE i, j;

    UBYTE sprite_id = 0x0;
    for(i = 0; i < NUM_ROWS; i++) {
        for(j = 0; j < NUM_COLS; j++) {
            blank(sprite_id);
            cat_table[i][j] = sprite_id;
            sprite_id += 0x2;
        }
    }
}

/*
 * Initializes the bucket array with empty sprites and no cats
 */
void init_buckets() {
    BYTE i;

    for(i = 0; i < NUM_COLS; i++) {
        buckets[i].cat_mem = BLANK_SPRITE_MEMORY;
        buckets[i].num_cats = 0;
    }
}

/*
 * Randomly assigns a different set of cat tiles to a given sprite ID
 */
void assign_new_cat(UBYTE sprite_id) {
    // TODO: Make this more random
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

/*
 * Erases a rectangular area of the tile map
 */
void clear(UBYTE x, UBYTE y, UBYTE w, UBYTE h) {
    UBYTE i, j;
    UBYTE tile = BLANK_TILE;

    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            set_bkg_tiles(x + i, y + j, 1, 1, &tile);
        }
    }
}

/*
 * Changes the tiles associated with the bottom cat of a given column to be
 * blank. If the entire column is blank, does nothing.
 */
void remove_bottom_cat(UBYTE column) {
    UBYTE cat, i;

    for(i = NUM_ROWS - 1; i >= 0; i--) {
        cat = get_sprite_tile(cat_table[i][column]);
        if (BLANK_SPRITE_MEMORY != cat) {
            blank(cat_table[i][column]);
            cats_changed = TRUE;
            return;
        }
    }
}

/*
 * Redraws the entire screen based on the current game state. Should only be
 * called during the vblank to ensure that there are no graphical errors.
 */
void render() {
    UBYTE x, y, x_offset, y_offset, cats_to_draw, cat_type, cat, tile;
    BYTE i, j;

    disable_interrupts();

    // Don't render if we don't have to
    if (!cats_changed) {
        return;
    }

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

    // Redraw the buckets
    for (i = 0; i < NUM_COLS; i++) {
        struct bucket_entry *entry = &buckets[i];
        cats_to_draw = entry->num_cats;
        cat_type = entry->cat_mem;

        // Find the top left corner of the bucket in the tile map
        x = (BUCKET_START_X + i * (COLUMN_SPACING + BUCKET_WIDTH)) / TILE_WIDTH - 1;
        y = BUCKET_START_Y / TILE_HEIGHT - 1;

        // Blank out the current bucket
        tile = BLANK_TILE;
        set_bkg_tiles(x, y, BUCKET_WIDTH / TILE_WIDTH,
                            BUCKET_HEIGHT / TILE_HEIGHT, &tile);

        // Erase any tiles currently in the bucket
        clear(x, y, BUCKET_WIDTH / TILE_WIDTH, BUCKET_HEIGHT / TILE_HEIGHT);

        // Draw the cats into the bucket
        for (cat = 0; cat < cats_to_draw; cat++) {
            x_offset = cat % 2;
            y_offset = cat / 2;

            // Select the appropriate tile for the cat
            if (cat_type == STRIPED_CAT_MEMORY) {
                tile = STRIPED_CAT_TILE;
            } else if(cat_type == BLACK_CAT_MEMORY) {
                tile = BLACK_CAT_TILE;
            } else if (cat_type == SIAMESE_CAT_MEMORY) {
                tile = SIAMESE_CAT_TILE;
            } else if (cat_type == FALLING_CAT_MEMORY) {
                tile = FALLING_CAT_TILE;
            }

            // Draw the cat into tile background memory
            set_bkg_tiles(x + x_offset, y + y_offset, 1, 1, &tile);
        }
    }

    // TODO: Draw score
    
    cats_changed = FALSE;
    enable_interrupts();
}

/*
 * Shifts the rows of the cat table down by one, and assigns the bottom row to
 * the top row.
 */
void update_cats() {
    UBYTE tmp [NUM_COLS];
    BYTE i, j;
    
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
    }

    cats_changed = TRUE;
}

/*
 * Wrapper to treat two 8x16 sprites as a 16x16 sprite
 */
void set_sprite_tile_16(UBYTE number, UBYTE tile) {
    const UBYTE TILE_OFFSET = 2;
    set_sprite_tile(number, tile);
    set_sprite_tile(number + 1, tile + TILE_OFFSET);
}

/*
 * Wrapper to treat two 8x16 sprites as a 16x16 sprite
 */
void move_sprite_16(UBYTE number, UBYTE x, UBYTE y) {
    const UBYTE X_OFFSET = 0x08;
    move_sprite(number, x, y);
    move_sprite(number + 1, x + X_OFFSET, y);
}

/*
 * Takes care of overhead necessary to begin play. This function will
 * - Initialize registers
 * - Load sprites and tiles into memory
 * - Initialize the game state
 * - Set the appropriate sprite mode (8x16)
 * - Register the vblank handler
 * - Turn on the display
 */
void init_gameplay() {
    LCDC_REG = 0x01;
    BGP_REG = OBP0_REG = OBP1_REG = 0xE4U;
    
    wait_vbl_done();
    DISPLAY_OFF;
    
    // Initialize random number generator with contents of DIV_REG
    seed.b.l = DIV_REG;
    seed.b.h = DIV_REG;
    initrand(seed.w);
    
    // Load sprites into VRAM
    set_sprite_data(BLANK_SPRITE_MEMORY, sizeof(blank16) / TILES_IN_SPRITE, blank16);
    set_sprite_data(STRIPED_CAT_MEMORY,  sizeof(cat0)    / TILES_IN_SPRITE, cat0);
    set_sprite_data(BLACK_CAT_MEMORY,    sizeof(cat1)    / TILES_IN_SPRITE, cat1);
    set_sprite_data(FALLING_CAT_MEMORY,  sizeof(cat2)    / TILES_IN_SPRITE, cat2);
    set_sprite_data(SIAMESE_CAT_MEMORY,  sizeof(cat3)    / TILES_IN_SPRITE, cat3);

    SPRITES_8x16;
    SHOW_SPRITES;
    
    // Load tiles into VRAM
    set_bkg_data(CAT_FACE_MEMORY, 4, faces);
    
    // TODO: Load background into VRAM
    SHOW_BKG;

    // We don't use the window tile memory
    HIDE_WIN;
    
   
    // Initialize game state
    init_cat_table();
    init_buckets();
    update_cats();

    add_VBL(render);

    wait_vbl_done();
    
    DISPLAY_ON;
}

void do_gameplay() {
    static UBYTE vblanks_since_input = 0;
    static UBYTE vblanks_since_update = 0;

    UBYTE buttons, column;

    BOOLEAN should_remove_column = FALSE;

    buttons = joypad();

    // TODO: delay between button presses
    switch(buttons) {
        case(J_LEFT):
            column = 0;
            should_remove_column = TRUE;
        break;
        
        case(J_DOWN):
            column = 1;
            should_remove_column = TRUE;
        break;
        
        case(J_RIGHT):
            column = 2;
            should_remove_column = TRUE;
        break;
    }

    if(should_remove_column) {
        remove_bottom_cat(column);
    }

    if (vblanks_since_update > TIME_UNTIL_UPDATE) {
        update_cats();
        vblanks_since_update = 0;
    }

    vblanks_since_input++;
    vblanks_since_update++;
}
