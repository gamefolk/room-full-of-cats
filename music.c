#include<gb/gb.h>

void init_music() {
    NR50_REG = 0xFF;
    NR51_REG = 0xFF;
    NR52_REG = 0x80;

    NR10_REG = 0;       // No frequency sweep
    NR11_REG = 0x40;    // Wave duty 50%
    NR12_REG = 0xF0;    // Maximum volume
}

// Convenient method of accessing notes
enum notes {
  C0, Cd0, D0, Dd0, E0, F0, Fd0, G0, Gd0, A0, Ad0, B0,
  C1, Cd1, D1, Dd1, E1, F1, Fd1, G1, Gd1, A1, Ad1, B1,
  C2, Cd2, D2, Dd2, E2, F2, Fd2, G2, Gd2, A2, Ad2, B2,
  C3, Cd3, D3, Dd3, E3, F3, Fd3, G3, Gd3, A3, Ad3, B3,
  C4, Cd4, D4, Dd4, E4, F4, Fd4, G4, Gd4, A4, Ad4, B4,
  C5, Cd5, D5, Dd5, E5, F5, Fd5, G5, Gd5, A5, Ad5, B5,
  SILENCE, END
};

// Convenient method of accessing appropriate frequencies
const UWORD frequencies[] = {
  44, 156, 262, 363, 457, 547, 631, 710, 786, 854, 923, 986,
  1046, 1102, 1155, 1205, 1253, 1297, 1339, 1379, 1417, 1452, 1486, 1517,
  1546, 1575, 1602, 1627, 1650, 1673, 1694, 1714, 1732, 1750, 1767, 1783,
  1798, 1812, 1825, 1837, 1849, 1860, 1871, 1881, 1890, 1899, 1907, 1915,
  1923, 1930, 1936, 1943, 1949, 1954, 1959, 1964, 1969, 1974, 1978, 1982,
  1985, 1988, 1992, 1995, 1998, 2001, 2004, 2006, 2009, 2011, 2013, 2015
};

// Represents the music to be played sequentially
const UBYTE music[] = {
    C0, D0, E0, F0, G0, A0, B0, C1, END
};

// Plays music with appropriate time between vblanks
void play_music(BOOLEAN should_loop) {
    UWORD freq;
    static UBYTE note_index = 0;
    static UBYTE vblanks = 0;
    static const int VBLANK_LIMIT = 30;


    vblanks++;
    if (vblanks > VBLANK_LIMIT) {
        if(music[note_index] == END) {
            if (should_loop) {
                note_index = 0;
            } else {
                NR12_REG = 0;
                return;
            }
        } 

        freq = frequencies[music[note_index]];
       
        NR13_REG = (unsigned char)freq;     // Low bits of frequency
        NR14_REG = 0x80 | (freq >> 8);      // High bits of frequency
        note_index++;
        
        vblanks = 0;
    }
}
