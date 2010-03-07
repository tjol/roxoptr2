CC	= $(PREFIX)gcc
LD	= $(CC)

CFLAGS	 = -g -O2 -Wall $(MACHDEP) $(INCLUDE) $(MACROS) $(SDL_CFLAGS)
LDFLAGS  = -g $(MACHDEP)
LIBS	 = $(shell sdl-config --libs) -lSDL_image -lSDL_ttf -lm

SDL_CFLAGS = $(shell sdl-config --cflags)

# ---------------------------------------------------------------------------

CFILES    = $(wildcard source/*.c)

OFILES    = $(CFILES:.c=.o)

# ---------------------------------------------------------------------------

ifeq ($(system),wii)
    ifeq ($(strip $(DEVKITPPC)),)
    $(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
    endif
    PORTLIBS:= $(DEVKITPRO)/portlibs/ppc
    PATH	:= $(DEVKITPPC)/bin:$(PORTLIBS)/bin:$(PATH)
    PREFIX	:= powerpc-eabi-
    MACHDEP	 = -DGEKKO -mrvl -mcpu=750 -meabi -mhard-float
    LIBS	:= -L$(PORTLIBS)/lib -L$(DEVKITPRO)/libogc/lib/wii \
		   -lSDL -lSDL_image -lSDL_ttf \
		   -lpng -ljpeg -lfreetype \
		   -lz -lfat -lwiiuse -lbte -logc -lm -lwiikeyboard
    INCLUDE  := -I$(PORTLIBS)/include -I$(DEVKITPRO)/libogc/include
    SDL_CFLAGS = 
    MACROS = -DWII
endif

# ---------------------------------------------------------------------------

build:
	$(MAKE) roxoptr2

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.h: %
	cd `dirname "$<"`; xxd -i `basename "$<"` > `basename "$<"`.h
	

source/display.o: source/display.c source/img/heli.png.h source/data/DejaVuSans-Bold.ttf.h

source/levels/lv0/lv0.o: source/levels/lv0/lv0.c source/levels/lv0/lv0.gif.h

roxoptr2.elf: $(OFILES) source/levels/lv0/lv0.o
	$(LD) $^ $(LDFLAGS) $(LIBS) -o $@

%.dol: %.elf
	elf2dol $< $@

roxoptr2: $(OFILES) source/levels/lv0/lv0.o source/levels/lv1/lv1.o
	$(LD) $(LDFLAGS) $(LIBS) $^ -o $@

wii:
	$(MAKE) system=wii roxoptr2.dol

    
clean:
	rm -f roxoptr2 source/*.o source/levels/lv0/lv0.o source/*~ roxoptr2.elf roxoptr2.dol source/img/*.h


.PHONY: clean
