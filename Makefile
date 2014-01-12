TARGET=cats.gb

CC=lcc
CFLAGS=-Iinclude -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG

LINKER = lcc -o
LFLAGS=$(CFLAGS)

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
