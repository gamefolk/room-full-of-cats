# cross-platform makefile requires gnuwin32 CoreUtils

TARGET=cats.gb

CC=lcc
CFLAGS=-Iinclude

LINKER = lcc -o
#       .lst  .map  ??    ROM only
LFLAGS= -Wa-l -Wl-m -Wl-j -Wl-yt0


SRCDIR = src
BINDIR = bin

SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=%.o)

# Make sure that Windows paths work with lcc
ifdef SystemRoot
	FixPath = $(subst /,\,$1)
	rm      = del /S
else
	FixPath = $1
	rm      = rm -f
endif

$(BINDIR)/$(TARGET): $(OBJECTS)
	test -d $(BINDIR) || mkdir $(BINDIR)
	$(LINKER) $@ $(LFLAGS) $(OBJECTS)

$(OBJECTS): %.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $(call FixPath,$<) -o $@

.PHONY: clean
clean:
	$(rm) $(OBJECTS)
	$(rm) $(wildcard *.lst)

.PHONY: remove
remove: clean
	$(rm) $(call FixPath,$(BINDIR)/$(TARGET))
