TARGET=cats.gb

#                          ??              .lst       .map       ??             ??                                    ROM only 
CC=lcc -Iinclude -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -Wl-yt0

LINKER = lcc -o

SRCDIR = src
BINDIR = bin

SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=%.o)

# Make sure that Windows paths work with lcc
ifdef SystemRoot
	FixPath = $(subst /,\,$1)
	rm      = del /S
	mkdir   = mkdir
else
	FixPath = $1
	rm      = rm -f
	mkdir   = mkdir -p
endif

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(mkdir) $(BINDIR)
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
