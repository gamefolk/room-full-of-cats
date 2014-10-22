#include <gb/gb.h>
#include <rand.h>

#include "text.h"

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
static void draw_buckets();
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
 * The cat faces, score, and time are drawn on the window,
 * and the pause message is drawn on the background (initially hidden off-screen)
 * When the user pauses, the background is scrolled back into the visible area.
 */

/*
 * The position of the window on the screen
 */
#define WIN_X            7                       /* (pixels) align the window to the left of the background */
#define WIN_Y            80                     /* (pixels) shift the window down so the pause message is visible */
#define WIN_TILE_Y    10                     /* WIN_Y / 8 pixels per tile

/*
 * Margin gives the spacing between the edges of the screen and where the rows
 * or columns should start. Padding is the spacing between rows or columns.
 */
#define COLUMN_PADDING  8
#define ROW_PADDING     8
#define ROW_MARGIN      24              /* Y_START + ROW_PADDING */
#define BUCKET_ROW      14              /* (ROW_MARGIN + 4 * (ROW_PADDING + SPRITE_HEIGHT) + ROW_PADDING) / 8 */

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
#define NUM_ROWS                    4
#define MAX_CATS_IN_BUCKET   4
#define MAX_COLUMNS               4
#define MAX_CATS                      16          /* NUM_ROWS * MAX_COLUMNS */

/*
 * Constants that affect gameplay
 */
#define VBLANK_TIME 60

static const char* text_score = "CATS:";
static const char* text_time = "TIME:";
static const char* text_time_free = "XXX";

static const char* text_pause1 = "SELECT";
static const char* text_pause2 = "TO END";

static bucket_t buckets[MAX_COLUMNS];

static void (*control_funcs[3])(UBYTE);

static UBYTE column_margin;
static UBYTE bucket_column;

static UBYTE num_columns;
static UBYTE num_cats;

static UBYTE vblank_speed;

static UBYTE last_row_id;

static UBYTE score;
static UBYTE time;

/*
 * Updates the buckets to contain the last cat in each column. If the bucket is
 * full, or contains a different type of cat, then the contents of the bucket
 * are replaced by the new cat.
 */
static void set_buckets() {
    UBYTE i;
    sprite_t cat_tile;

    cat_tile = get_cat_tile(last_row_id);

    for (i = 0; i < num_columns; i++) {
        cat_tile = get_cat_tile(last_row_id + i);
        if (buckets[i].cat_id == cat_tile || buckets[i].cat_id == BLANK) {
            if (buckets[i].num_cats < MAX_CATS_IN_BUCKET) {
                buckets[i].cat_id = cat_tile;
                if (cat_tile != BLANK) {
                    buckets[i].num_cats++;
                }
                continue;
            } else {
                score++;
                draw_ubyte_win(5, 17 - WIN_TILE_Y, score);
                /* Clear bucket */
                buckets[i].num_cats = 0;
                buckets[i].cat_id = BLANK;
            }
#ifndef BLANKS_RESET_BUCKETS
        } else if (cat_tile != BLANK) {
            buckets[i].num_cats = 1;
            buckets[i].cat_id = cat_tile;
        }
#else
        } else {
            if (cat_tile == BLANK) {
                buckets[i].num_cats = 0;
                buckets[i].cat_id = BLANK;
            } else {
                buckets[i].num_cats = 1;
                buckets[i].cat_id = cat_tile;
            }
        }
#endif
    }
    draw_buckets();
}

static void draw_buckets() {
    UBYTE i, j;
    UBYTE bucket_x, bucket_y, x_offset, y_offset;
    sprite_t cat_tile;
    cat_face_t face_to_draw;

    for (i = 0; i < num_columns; i++) {
        cat_tile = buckets[i].cat_id;
        bucket_x = bucket_column + 3 * i;
        bucket_y = BUCKET_ROW;
        for (j = 0; j < MAX_CATS_IN_BUCKET; j++) {
            x_offset = (j % 2);
            y_offset = (j / 2);

            if (j < buckets[i].num_cats) {
                face_to_draw = get_cat_face(cat_tile);
            } else {
                face_to_draw = BLANK_CAT_FACE;
            }
            draw_cat_face(bucket_x + x_offset, bucket_y + y_offset,
                          face_to_draw);
        }
    }
}

/*
 * Translates a cat sprite tile ID into a cat face background tile address
 */
static cat_face_t get_cat_face(sprite_t cat_tile) {
    switch (cat_tile) {
    case BLANK:
        return BLANK_CAT_FACE;
    case STRIPED_CAT:
        return STRIPED_CAT_FACE;
    case BLACK_CAT:
        return BLACK_CAT_FACE;
    case FALLING_CAT:
        return FALLING_CAT_FACE;
    case SIAMESE_CAT:
        return SIAMESE_CAT_FACE;
    }
}


/*
 * Sets the top row of sprites to be a new row of random cats.
 */
static void start_row() {
    UBYTE i;
    for (i = 0; i < num_columns; i++) {
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
    for (i = num_cats - 1; i >= num_columns; i--) {
        /* Change the current cat's tile to the cat above it. */
        change_cat(i, get_cat_tile(i - num_columns));
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
 * Convenience function to draw a cat face tile to the window, offset such that it appears in background space
 */
static void draw_cat_face(UBYTE x, UBYTE y, cat_face_t cat_face) {
    set_win_tiles(x, y - WIN_TILE_Y, 1, 1, (unsigned char*)&cat_face);
}

static void control_2(UBYTE buttons) {
    switch(buttons) {
        case (J_B):
            change_cat(last_row_id, BLANK);
        break;

        case (J_A):
            change_cat(last_row_id + 1, BLANK);
        break;
    }
}

static void control_3(UBYTE buttons) {
    switch(buttons) {
        case (J_LEFT):
            change_cat(last_row_id, BLANK);
        break;

        case (J_B):
            change_cat(last_row_id + 1, BLANK);
        break;

        case (J_A):
            change_cat(last_row_id + 2, BLANK);
        break;
    }
}

static void control_4(UBYTE buttons) {
    switch(buttons) {
        case (J_LEFT):
            change_cat(last_row_id, BLANK);
        break;

        case (J_RIGHT):
            change_cat(last_row_id + 1, BLANK);
        break;

        case (J_B):
            change_cat(last_row_id + 2, BLANK);
        break;

        case (J_A):
            change_cat(last_row_id + 3, BLANK);
        break;
    }
}

/*
 * Called by the game loop when do_gameplay() requests a pause
 * Returns a flag indicating whether the game should continue (TRUE) or quit (FALSE)
 */
BOOLEAN pause_game() {
    move_bkg(0, 0);
    waitpadup();

    while (!(joypad() & J_START)) {
        /* quit */
        if (joypad() & J_SELECT) {
            return FALSE;
        }
    }

    waitpadup();
    move_bkg(144, 0);

    /* continue running */
    return TRUE;
}

/*
 * Called before entering the main program loop
 * Sets the LCD and palette registers for both the game AND the tutorial screen
 * Loads the font, sprite, and background tiles
 * Positions the window and draws the window UI (score and time indicators)
 * Seeds the random number generator
 */
void load_game() {
    fixed seed;

    /*
     * LCD display                    = Off
     * Window tile map            = 0x9C00-0x9FFF
     * Window display             = On
     * BG tile map                    = 0x9800-0x9BFF
     * BG & Window tile data = 0x8800-0x97FF
     * Sprite size                     = 8x16
     * Sprite display                = On
     * BG display                      = On
     */
    LCDC_REG = 0x67;

    /* Set palettes */
    BGP_REG = OBP0_REG = OBP1_REG = 0xE4U;

    /* set control function pointers */
    control_funcs[0] = control_2;
    control_funcs[1] = control_3;
    control_funcs[2] = control_4;

    load_font();

    /* Load sprite tiles */
    set_sprite_data(BLANK,       0x04, (UBYTE*)blank16);
    set_sprite_data(STRIPED_CAT, 0x04, (UBYTE*)cat0);
    set_sprite_data(BLACK_CAT,   0x04, (UBYTE*)cat1);
    set_sprite_data(FALLING_CAT, 0x04, (UBYTE*)cat2);
    set_sprite_data(SIAMESE_CAT, 0x04, (UBYTE*)cat3);

    /* Load background tiles. We can read all of the small cat faces into
     * memory at once because they are next to each other starting at 0x04.
     */
    set_bkg_data(BLANK_CAT_FACE,   0x01, (UBYTE*)blank8);
    set_bkg_data(SIAMESE_CAT_FACE, 0x04, (UBYTE*)faces);

    /* position the window */
    move_win(WIN_X, WIN_Y);

    /* draw the UI on the window */
    draw_text_win(0, 17 - WIN_TILE_Y, text_score);
    draw_text_win(12, 17 - WIN_TILE_Y, text_time);

    /* Initialize random number generator with contents of DIV_REG */
    seed.b.l = DIV_REG;
    seed.b.h = DIV_REG;
    initrand(seed.w);
}

/*
 * Called by the main program loop before each game loop.
 * Initializes the game based on the options set in the tutorial screen.
 * Draws the pause message on the background and hides it
 * Creates and draws the cat sprites
 */
void init_gameplay(UBYTE* options) {
    UBYTE i, j;
    UBYTE x_pos, y_pos;
    UBYTE cat_number;

    /* clear the buckets from the last game */
    for (i = 0; i < MAX_COLUMNS; i++) {
        buckets[i].num_cats = 0;
        buckets[i].cat_id = BLANK;
    }

    draw_buckets();

    /* set game options */
    num_columns = options[0];
    num_cats = num_columns * NUM_ROWS;
    last_row_id = num_columns * 3;

    column_margin = (X_END / 2) - ((SPRITE_WIDTH * num_columns) + (COLUMN_PADDING * num_columns) / 2) + X_START;
    bucket_column = (column_margin - 1) / 8;

	vblank_speed = options[1];
    time = options[2]; /* if time = 255, free play */

    score = 0;
    draw_ubyte_win(5, 17 - WIN_TILE_Y, score);

    if (time != 255) {
        draw_ubyte_win(17, 17 - WIN_TILE_Y, time);
    } else {
        draw_text_win(17, 17 - WIN_TILE_Y, text_time_free);
    }

    /* draw the pause message on the background, and hide it */
    draw_text(7, 8, text_pause1);
    draw_text(7, 9, text_pause2);
    move_bkg(144, 0);

    /* Create all the sprites (2 for each cat), make them blank, and set them to draw behind the background */
    for (i = 0; i < MAX_CATS * 2; i++) {
        set_sprite_tile(i, BLANK);
        set_sprite_prop(i, 0x80);
    }

    /* Draw cat sprite locations */
    for (i = 0; i < NUM_ROWS; i++) {
        for (j = 0; j < num_columns; j++) {
            x_pos = column_margin;
            x_pos += j * (COLUMN_PADDING + CAT_WIDTH);

            y_pos = ROW_MARGIN;
            y_pos += i * (ROW_PADDING + CAT_HEIGHT);

            cat_number = i * num_columns + j;

            draw_cat(cat_number, x_pos, y_pos);
        }
    }
}

/*
 * Called by the game loop to effect game logic
 * Returns a flag indicating whether the game should continue (0), pause (1), or quit (2)
 */
UBYTE do_gameplay() {
    static BOOLEAN paused = FALSE;
    static UBYTE vblanks_time = 0;
	static UBYTE vblanks_speed = 0;

    vblanks_time++;
	vblanks_speed++;

    control_funcs[num_columns - 2](joypad());

    if (vblanks_speed > vblank_speed) {
        vblanks_speed = 0;

        set_buckets();
        shift_rows();
        start_row();
    }

	if (vblanks_time > VBLANK_TIME) {
		vblanks_time = 0;

        if (time == 0) {
            move_bkg(0, 0);

            while (!(joypad() & J_SELECT)) {}
                /* quit */
                return 2;
        }

        if (time != 255) {
            time--;
            draw_ubyte_win(17, 17 - WIN_TILE_Y, time);
        }
	}

    /* pause */
    if (joypad() & J_START) {
        return 1;
    }

    /* continue running */
    return 0;
}
