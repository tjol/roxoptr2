if ON_WII
noinst_PROGRAMS = boot.elf boot.dol
else
bin_PROGRAMS = roxoptr2
endif

CPPFLAGS = -DROXOPTR2_DATADIR=\"$(pkgdatadir)\"

roxoptr2_SOURCES = main.c main.h display.c display.h game.c game.h \
		   levels.c level.h menu.c menu.h \
		   sprite.c sprite.h overlays.c overlays.h \
		   cfgparser.c cfgparser.h filesys.c filesys.h \
		   levels/level_tpl.h levels/lv0/lv0.c levels/lv0/lv0.xbm
nodist_roxoptr2_SOURCES = data/DejaVuSans-Bold.ttf.h img/heli.png.h \
			  levels/lv0/lv0.gif.h

boot_elf_SOURCES = $(roxoptr2_SOURCES)
nodist_boot_elf_SOURCES = $(nodist_roxoptr2_SOURCES)
boot_dol_SOURCES = 

BUILT_SOURCES = data/DejaVuSans-Bold.ttf.h img/heli.png.h \
		levels/lv0/lv0.gif.h

EXTRA_DIST = data/DejaVuSans-Bold.ttf img/heli.png \
			 levels/lv0/lv0.gif
CLEANFILES = data/DejaVuSans-Bold.ttf.h img/heli.png.h \
	     levels/lv0/lv0.gif.h

XTOHTRICK = mkdir -p `dirname $@`; \
	    mangled_name=`echo $? | sed 's,.*/,,;s/[ .,-]/_/g'` ; \
	    echo >$@ "unsigned char $${mangled_name}[] = {" ; \
	    xxd -i < $? >> $@ ; \
	    echo >>$@ "};" ; \
	    echo >>$@ "unsigned int $${mangled_name}_len = " `wc -c $? | awk '{print $$1;}'` ";"

# This could be done with %.h: %, but the % syntax is a GNU
# extension, and we don't want to exclude other UNIX
# make programs (ex: FreeBSD)

data/DejaVuSans-Bold.ttf.h: data/DejaVuSans-Bold.ttf
	$(XTOHTRICK)

img/heli.png.h: img/heli.png
	$(XTOHTRICK)

levels/lv0/lv0.gif.h: levels/lv0/lv0.gif
	$(XTOHTRICK)

if ON_WII
boot.dol: boot.elf
	$(DEVKITPPC)/bin/elf2dol boot.elf boot.dol
endif


