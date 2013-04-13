lcc -Wa-l -DUSE_SFR_FOR_REG -c -o game.o game.c
lcc -Wa-l -DUSE_SFR_FOR_REG -c -o music.o music.c
lcc -Wa-l -DUSE_SFR_FOR_REG -Wl-yt2 -Wl-yo4 -Wl-yp0x143=0x00 -o cats.gb game.o music.o
del *.o
del *.lst
del *.map
