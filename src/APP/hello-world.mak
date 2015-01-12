# Link & run:
#   make run -f hello-world.mak
# Menu on Ubuntu: export UBUNTU_MENUPROXY=


# get OS OUTDIR VGUI (Version GTK)
include ../options.mak


SRC1 =\
 hello-world.c


OBJ1 = $(patsubst %.c,%.o, $(notdir $(SRC1)))
OBJGUI = $(patsubst %.c,%.o, $(notdir $(SRCGUI)))

GUICP = -D$(VGUI)
CPDEB = -DDEB -ggdb -pg
LKDEB = -ggdb -pg

VPATH = $(OUTDIR):../ut/:../gui_$(VGUI)/:.

OGLLIB = -lGLU -lGL


default: $(OBJ1) $(OBJGUI)
	cd $(OUTDIR) && \
	$(CC) $(OBJ1) libgui_$(VGUI).so $(OGLLIB) $(LKDEB)


run:
	make -f hello-world.mak
	export LD_LIBRARY_PATH=$(OUTDIR) && $(OUTDIR)/a.out


.c.o:
	$(CC) -c $(GUICP) $(CPDEB) $<
	mv -f $(@F) $(OUTDIR)/.
	ctags -f $*.tag $<

# eof
