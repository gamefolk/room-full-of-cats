
lcc -Wa-l -DUSE_SFR_FOR_REG -c main.c
lcc -Wa-l -DUSE_SFR_FOR_REG -c gameplay.c
lcc -Wa-l -DUSE_SFR_FOR_REG -c music.c
lcc -Wa-l -DUSE_SFR_FOR_REG -Wl-yt2 -Wl-yo4 -Wl-yp0x143=0x00 -o cats.gb main.o music.o gameplay.o
del *.o
del *.lst
del *.map
