lcc -Wa-l -DUSE_SFR_FOR_REG -c -o main.o main.c
lcc -Wa-l -DUSE_SFR_FOR_REG -c -o music.o music.c
lcc -Wa-l -DUSE_SFR_FOR_REG -c -o gameplay.o gameplay.c
lcc -Wa-l -DUSE_SFR_FOR_REG -Wl-yt2 -Wl-yo4 -Wl-yp0x143=0x00 -o test.gb main.o music.o gameplay.o
del *.o
del *.lst
del *.map
