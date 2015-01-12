# Link & run:
#   make run -f tst_lst1.mak
# Menu on Ubuntu: export UBUNTU_MENUPROXY=
# export LD_LIBRARY_PATH=../binLinux32 && gdb ../binLinux32/a.out

# name of this makefile
PRJNAM = tst_lst1

# all sourcefiles
SRC1 =\
 tst_lst1.c\


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
