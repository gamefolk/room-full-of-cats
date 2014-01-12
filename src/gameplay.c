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

UWORD colX, colY, sprID, tileID;
UWORD buckets[4];
UWORD score[4];

fixed seed;

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
    cur += colNum * (NUM_COLUMNS * 0x08);
    cur += colY * 0x08;

    last = *(UBYTE*)(cur);

    *(UBYTE*)(cur) = sprID;
    *(UBYTE*)(cur + 0x04) = sprID + 0x02;

    return last;
}

void setBuckets(UWORD colNum) {
    if (sprID == STRIPED_CAT) {
        if (buckets[colNum] == 0x03) {
            score[colNum] = 0x03;
        } else {
            buckets[colNum] = 0x03;
            score[colNum] = 0;
        }
    } else if (sprID == BLACK_CAT) {
        if (buckets[colNum] == 0x04) {
            score[colNum] = 0x04;
        } else {
            buckets[colNum] = 0x04;
            score[colNum] = 0;
        }
    } else if (sprID == FALLING_CAT) {
            if (buckets[colNum] == 0x02) {
                score[colNum] = 0x02;
            } else {
                buckets[colNum] = 0x02;
                score[colNum] = 0;
            }
    } else if (sprID == SIAMESE_CAT) {
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
        return STRIPED_CAT;
    else if (gen & 4)
        return BLACK_CAT;
    else if (gen & 16)
        return FALLING_CAT;
    else
        return SIAMESE_CAT;
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

void init_gameplay() {
    UWORD i;

    disable_interrupts();
    DISPLAY_OFF;

    LCDC_REG = 0x67;
    /*
     * LCD        = Off
     * WindowBank = 0x9C00-0x9FFF
     * Window     = On
     * BG Chr     = 0x8800-0x97FF
     * BG Bank    = 0x9800-9BFF
     * OBJ        = 8x16
     * OBJ        = On
     * BG         = On
     */

    // Set palettes
    BGP_REG = OBP0_REG = OBP1_REG = 0xE4U;

    // load sprite tiles
    set_sprite_data(0x00, 0x04, blank16);
    set_sprite_data(0x04, 0x04, cat0);
    set_sprite_data(0x08, 0x04, cat1);
    set_sprite_data(0x0C, 0x04, cat2);
    set_sprite_data(0x10, 0x04, cat3);

    // Create all the sprites. 2 for each cat.
    for (i = 0; i < 32; i++) {
        set_sprite_tile(i, i * 2);
    }

    SHOW_SPRITES;

    // Load background tiles
    set_bkg_data(0x00, 0x01, blank8);
    set_bkg_data(0x01, 0x04, faces);

    DISPLAY_ON;
    enable_interrupts();

    // Initialize random number generator with contents of DIV_REG
    seed.b.l = DIV_REG;
    seed.b.h = DIV_REG;
    initrand(seed.w);

    // set up columns

    colX = 0x2B;

    for (i = 0; i < 3; i++) {
        makeColumn(i);
        colX += 0x18;
    }

    startRow();


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

    if (vblanks > VBLANK_UPDATE) {
        vblanks = 0;

        for (i = 0; i < 4; i++) {
            UWORD columnOffset;
            columnOffset = 3 * i ;
            *(UWORD*)(0x9800 + 0x1C0 + 0x04 + columnOffset) = buckets[i];
            *(UWORD*)(0x9801 + 0x1C0 + 0x04 + columnOffset) = score[i];
        }

        moveRows();
        startRow();
    }
}
