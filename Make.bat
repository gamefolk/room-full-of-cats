
lcc -Wa-l -DUSE_SFR_FOR_REG -c src\main.c
lcc -Wa-l -DUSE_SFR_FOR_REG -c src\gameplay.c
lcc -Wa-l -DUSE_SFR_FOR_REG -c src\music.c
lcc -Wa-l -DUSE_SFR_FOR_REG -Wl-yt2 -Wl-yo4 -Wl-yp0x143=0x00 -o bin\cats.gb main.o music.o gameplay.o
del *.o
del *.lst
del *.map
