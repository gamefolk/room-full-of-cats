#include <gb/gb.h>
#include <rand.h>

#include "tiles/fallcats.c"
#include "tiles/blank.c"
#include "tiles/stackcats.c"
#include "tiles/background.c"
#include "tiles/bgtiles.c"

/*
 * Tile IDs for each cat sprite.
 */
typedef enum {
    BLANK       = 0x00,
    STRIPED_CAT = 0x04,
    BLACK_CAT   = 0x08,
    FALLING_CAT = 0x0C,
    SIAMESE_CAT = 0x10
} sprite_t;

/*
 * Tile IDs for each cat face background tile.
 */
typedef enum {
    BLANK_CAT_FACE   = 0x00,
    STRIPED_CAT_FACE = 0x03,
    BLACK_CAT_FACE   = 0x04,
    FALLING_CAT_FACE = 0x02,
    SIAMESE_CAT_FACE = 0x01
} cat_face_t;

/*
 * Stores the bucket information (the type of cat, as well as the numer of
 * cats) in a simple data wrapper.
 */
typedef struct bucket_t {
    sprite_t cat_id;
    UBYTE num_cats;
} bucket_t;

/*
 * Function prototypes
 */
static void set_buckets();
static void start_row();
static void shift_rows();
static sprite_t get_cat_tile(UBYTE);
static sprite_t pickCat();
static cat_face_t get_cat_face(sprite_t);
static void change_cat(UBYTE, UBYTE);
static void draw_cat_face(UBYTE, UBYTE, cat_face_t);
static void draw_cat(UBYTE, UBYTE, UBYTE);

/*
 * Constants useful for drawing sprites. Note that SDCC cannot determine
 * constant expressions correctly (!) so we have to do some math to determine
 * what the correct values should be.
 *
 * It is possible to draw sprites off the screen, so the first pixels that show
 * the entire sprite are given by X_START and Y_START. Similarly, the last
 * pixels that a sprite may be drawn at for it to be on the entire screen are
 * given by X_END and Y_END.
 */
#define X_START         8
#define X_END           168
#define Y_START         16
#define Y_END           160

/*
 * Margin gives the spacing between the edges of the screen and where the rows
 * or columns should start. Padding is the spacing between rows or columns.
 */
#define COLUMN_PADDING  8
#define COLUMN_MARGIN   40              /* X_START + 4 * COLUMN_PADDING */
#define ROW_PADDING     8
#define ROW_MARGIN      24              /* Y_START + ROW_PADDING */
#define BUCKET_ROW      14              /* (ROW_MARGIN + 4 * (ROW_PADDING + SPRITE_HEIGHT) + ROW_PADDING) / 8 */
#define BUCKET_COLUMN   4               /* (COLUMN_MARGIN - 1) / 8 */

/*
 * Room full of cats uses 8x16 sprites, but the cats themselves are represented
 * by 2 8x16 (left and right halves) to make a single 16x16 sprites. "Cat"
 * refers to the 16x16 sprite, while "sprite" refers to the 8x16 tile used by
 * GBDK.
 */
#define SPRITE_WIDTH    8
#define SPRITE_HEIGHT   16
#define CAT_WIDTH       16              /* SPRITE_WIDTH * 2 */
#define CAT_HEIGHT      16              /* SPRITE_HEIGHT */

/*
 * Constants that describe gameplay elements.
 */
#define NUM_ROWS        4
#define NUM_COLUMNS     4
#define NUM_CATS        16              /* NUM_ROWS * NUM_COLUMNS */

/*
 * Constants that affect gameplay.
 */
#define VBLANK_UPDATE   60              /* Vblanks until gameplay update */

static bucket_t buckets[NUM_CATS];

/*
 * Updates the buckets to contain the last cat in each column. If the bucket is
 * full, or contains a different type of cat, then the contents of the bucket
 * are replaced by the new cat.
 */
static void set_buckets() {
    UBYTE i;
    sprite_t cat_tile;
    UBYTE bucket_x, bucket_y;

    cat_tile = get_cat_tile(12);

    for (i = 0; i < NUM_COLUMNS; i++) {
        cat_tile = get_cat_tile(12 + i);

        bucket_x = BUCKET_COLUMN + 3 * i;
        bucket_y = BUCKET_ROW;
        draw_cat_face(bucket_x, bucket_y, get_cat_face(cat_tile));
    }
}

/*
 * Translates a cat sprite tile ID into a cat face background tile address
 */
static cat_face_t get_cat_face(sprite_t cat_tile) {
    switch(cat_tile) {
        case BLANK:         return BLANK_CAT_FACE;
        case STRIPED_CAT:   return STRIPED_CAT_FACE;
        case BLACK_CAT:     return BLACK_CAT_FACE;
        case FALLING_CAT:   return FALLING_CAT_FACE;
        case SIAMESE_CAT:   return SIAMESE_CAT_FACE;
    }
}


/*
 * Sets the top row of sprites to be a new row of random cats.
 */
static void start_row() {
    UBYTE i;
    for (i = 0; i < NUM_COLUMNS; i++) {
        change_cat(i, pickCat());
    }
}

/*
 * Copies the cat sprites from the row above into each row of cats.
 */
static void shift_rows() {
    UBYTE i;

    /*
     * Iterate backwards through the cats. Stop at the first row, because the
     * first row of cats will be replaced with new cats anyways.
     */
    for (i = NUM_CATS - 1; i >= NUM_COLUMNS; i--) {
        /* Change the current cat's tile to the cat above it. */
        change_cat(i, get_cat_tile(i - NUM_COLUMNS));
    }
}

/*
 * Convenience method to get the tile number of a cat.
 */
static sprite_t get_cat_tile(UBYTE nb) {
    return get_sprite_tile(nb * 2);
}

/*
 * Returns a random cat ID.
 */
static sprite_t pickCat() {
    UINT8 gen = rand();

    if (gen & 1)
        return STRIPED_CAT;
    else if (gen & 4)
        return BLACK_CAT;
    else if (gen & 16)
        return FALLING_CAT;
    else
        return SIAMESE_CAT;
}

/*
 * Change the sprite tiles used by a cat to new sprite tiles.
 */
static void change_cat(UBYTE cat_number, UBYTE sprite_tile) {
    set_sprite_tile(cat_number * 2, sprite_tile);
    set_sprite_tile(cat_number * 2 + 1, sprite_tile + 0x2);
}

/*
 * Convenience function to draw a cat sprite to the screen. Each cat_number
 * actually refers to two 8x16 sprites, but the two sprites are drawn together
 * as a unit.
 */
static void draw_cat(UBYTE cat_number, UBYTE x, UBYTE y) {
    move_sprite(cat_number * 2, x, y);
    move_sprite(cat_number * 2 + 1, x + SPRITE_WIDTH, y);
}

/*
 * Convenience function to draw a cat face tile to the background.
 */
static void draw_cat_face(UBYTE x, UBYTE y, cat_face_t cat_face) {
    set_bkg_tiles(x, y, 1, 1, (unsigned char*)&cat_face);
}

/*
 * Initializes the state of the game. Should be called before the program
 * enters the game loop. It ensures that the appropriate registers are
 * initialized for displaying graphics and that the random number generator is
 * seeded.
 */
void init_gameplay() {
    UBYTE i, j;
    UBYTE x_pos, y_pos;
    UBYTE cat_number;
    fixed seed;

    disable_interrupts();
    DISPLAY_OFF;

    LCDC_REG = 0x6F;
    /*
     * LCD        = Off
     * WindowBank = 0x9C00-0x9FFF
     * Window     = On
     * BG Chr     = 0x8800-0x97FF
     * BG Bank    = 0x9C00-0x9FFF -- may cause problems for window memory,
     *                               but for now it allows the background to
     *                               show
     * OBJ        = 8x16
     * OBJ        = On
     * BG         = On
     */

    /* Set palettes */
    BGP_REG = OBP0_REG = OBP1_REG = 0xE4U;

    /* Load sprite tiles */
    set_sprite_data(BLANK,       0x04, (UBYTE*)blank16);
    set_sprite_data(STRIPED_CAT, 0x04, (UBYTE*)cat0);
    set_sprite_data(BLACK_CAT,   0x04, (UBYTE*)cat1);
    set_sprite_data(FALLING_CAT, 0x04, (UBYTE*)cat2);
    set_sprite_data(SIAMESE_CAT, 0x04, (UBYTE*)cat3);

    /* Create all the sprites and make them blank. 2 for each cat. */
    for (i = 0; i < NUM_CATS * 2; i ++) {
        set_sprite_tile(i, BLANK);
    }

    SHOW_SPRITES;

    /* Load background tiles. We can read a all of the small cat faces into
     * memory at once because they are next to each other starting at 0x04.
     */
    set_bkg_data(BLANK_CAT_FACE,   0x01, (UBYTE*)blank8);
    set_bkg_data(SIAMESE_CAT_FACE, 0x04, (UBYTE*)faces);

    SHOW_BKG;

    DISPLAY_ON;
    enable_interrupts();

    /* Draw cat sprite locations */
    for (i = 0; i < NUM_ROWS; i++) {
        for (j = 0; j < NUM_COLUMNS; j++) {
            x_pos = COLUMN_MARGIN;
            x_pos += j * (COLUMN_PADDING + CAT_WIDTH);

            y_pos = ROW_MARGIN;
            y_pos += i * (ROW_PADDING + CAT_HEIGHT);

            cat_number = i * NUM_COLUMNS + j;

            draw_cat(cat_number, x_pos, y_pos);
        }
    }

    /* Initialize random number generator with contents of DIV_REG */
    seed.b.l = DIV_REG;
    seed.b.h = DIV_REG;
    initrand(seed.w);
}

void do_gameplay() {
    static UBYTE vblanks = 0;
    UBYTE buttons;

    vblanks++;

    buttons = joypad();
    switch(buttons) {
        case(J_LEFT):
            change_cat(8, BLANK);
        break;

        case(J_DOWN):
            change_cat(9, BLANK);
        break;

        case(J_UP):
            change_cat(10, BLANK);
        break;

        case(J_RIGHT):
            change_cat(11, BLANK);
        break;
    }

    if (vblanks > VBLANK_UPDATE) {
        vblanks = 0;

        set_buckets();
        shift_rows();
        start_row();
    }
}
