CC=lcc
CFLAGS=-Wa-l -Iinclude

# Make sure that Windows paths work with lcc
ifdef SystemRoot
	FixPath = $(subst /,\,$1)
else
	FixPath = $1
endif

all: main.o music.o gameplay.o
	$(CC) $(CFLAGS) main.o music.o gameplay.o -o $(call FixPath,bin/cats.gb)

main.o: src/main.c
	$(CC) $(CFLAGS) -c $(call FixPath,src/main.c)

music.o: src/music.c
	$(CC) $(CFLAGS) -c $(call FixPath,src/music.c)

gameplay.o: src/gameplay.c
	$(CC) $(CFLAGS) -c $(call FixPath,src/gameplay.c)

clean:
	rm -rf *.o bin/cats.gb
