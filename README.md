# Room Full of Cats

*Room Full of Cats* is a native game for the Nintendo GameBoy, employing pixel
art and authentic chiptune. It first debuted at the [5C Hackathon], winning the
"Best Game" prize.

## Compiling

Room Full of Cats uses the [GBDK] tool chain to cross-compile to z80 assembly
for the GameBoy. To compile to a ROM from source, install [cmake] and GBDK,
ensuring that both `cmake` and `lcc` are on your PATH. Next, enter

```
cmake -G "Unix Makefiles"
make
```

into your terminal or command prompt to compile. The game will be compiled as
`cats.gb` in the project root.

## Running the Game

Room Full of Cats runs best on the [BGB] emulator, both natively and in [WINE].
The game will also run on the actual GameBoy hardware by transferring the ROM to
a Flash cartridge.

## Notes

The GBDK tool chain was last updated in 2001, so expect some possible
installation issues. GBDK is also available as an [AUR package] for Arch Linux
users.


[5C Hackathon]: http://www.5chackathon.com
[cmake]: http://www.cmake.org
[GBDK]: http://gbdk.sourceforge.net
[BGB]: http://bgb.bircd.org/
[WINE]: http://www.winehq.org/
[AUR package]: http://aur.archlinux.org/packages/gbdk/
