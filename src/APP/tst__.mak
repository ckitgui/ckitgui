
# get OS OUTDIR VGTK (Version GTK)
include ../options.mak


OBJ1 = $(patsubst %.c,%.o, $(notdir $(SRC1)))
OBJGUI = $(patsubst %.c,%.o, $(notdir $(SRCGUI)))

GUICP = -D$(VGUI)
CPDEB = -DDEB -ggdb -pg
LKDEB = -ggdb -pg
LKDEF = -lm -ldl

VPATH = $(OUTDIR):../ut/:../gui_$(VGUI)/:.

OGLLIB = -lGLU -lGL


default: $(OBJ1) $(OBJGUI)
	cp -f list2.dat $(OUTDIR)/.
	cd $(OUTDIR) && \
	$(CC) $(OBJ1) libgui_$(VGUI).so $(OGLLIB) $(LKDEF) $(LKDEB)


run:
	make -f $(PRJNAM).mak
	export LD_LIBRARY_PATH=$(OUTDIR) && $(OUTDIR)/a.out


.c.o:
	$(CC) -c $(GUICP) $(CPDEB) $<
	mv -f $(@F) $(OUTDIR)/.
	ctags -f $*.tag $<

# eof
