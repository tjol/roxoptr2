CC	= $(PREFIX)gcc
LD	= $(CC)

CFLAGS	 = -g -O2 -Wall $(STD_FLAGS) $(MACHDEP) $(INCLUDE) $(MACROS) $(SDL_CFLAGS)
STD_FLAGS = -std=c99 -pedantic -D_BSD_SOURCE=1
LDFLAGS  = -g $(MACHDEP)
LIBS	 = -lSDL -lSDL_image -lSDL_ttf -lm

#SDL_CFLAGS = $(shell sdl-config --cflags)
SDL_CFLAGS = 

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
    STD_FLAGS := -std=gnu99
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
	

source/display.o: source/display.c source/data/DejaVuSans-Bold.ttf.h
source/sprite.o: source/img/heli.png.h 

source/levels/lv0/lv0.o: source/levels/lv0/lv0.c source/levels/lv0/lv0.gif.h

roxoptr2.elf: $(OFILES) source/levels/lv0/lv0.o
	$(LD) $^ $(LDFLAGS) $(LIBS) -o $@

%.dol: %.elf
	elf2dol $< $@

roxoptr2: $(OFILES) source/levels/lv0/lv0.o
	$(LD) $(LDFLAGS) $(LIBS) $^ -o $@

wii:
	$(MAKE) system=wii roxoptr2.dol

    
clean:
	rm -f roxoptr2 source/*.o source/levels/*/*.o source/*~ roxoptr2.elf roxoptr2.dol source/img/*.h source/data/*.h


.PHONY: clean
