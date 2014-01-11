CC=/opt/gbdk/bin/lcc
CFLAGS=-Wa-l -Iinclude

all: main.o music.o gameplay.o
	$(CC) $(CFLAGS) main.o music.o gameplay.o -o bin/cats.gb

main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c

music.o: src/music.c
	$(CC) $(CFLAGS) -c src/music.c

gameplay.o: src/gameplay.c
	$(CC) $(CFLAGS) -c src/gameplay.c

clean:
	rm -rf *.o bin/cats.gb
