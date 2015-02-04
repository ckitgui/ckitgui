# Create GUI-dll for Gtk:   make -f ckitgui.mak
# delete obj's:             make remove -f ckitgui.mak
# check config:             vi options.mak


# get OS OUTDIR VGUI (Version GTK)
include ../options.mak


# Remove all objects (change gtk-version)
#   make remove -f c-kit-gui-gtk.mak


# get SRCGUI (filList), GUICP (compile with gui), GUILIB (link with gui)
include ../gui/gui_$(VGUI).mak


DLLNAM := libgui_$(VGUI)


# add utility-funcs
SRC1 := $(SRCGUI)\
 ../ut/ut_umem.c\
 ../ut/ut_umb.c\
 ../ut/ckit_ut.c\
 ../ut/ckit_os_ut_x.c


# dirs
DIRSRC1 = ../gui:../gui_$(VGUI):../ut


OBJ1 = $(patsubst %.c,%.o, $(notdir $(SRC1)))
OBJGUI = $(patsubst %.c,%.o, $(notdir $(SRCGUI)))


# Debug 1=Development; Debug-ON
#       2=Distribute;  Debug-OFF
DEB   = 1
CPDEB = -DDEB -ggdb -pg
LKDEB = -ggdb -pg


# OpenGL-binding: OGLLIB SRCGLB GLBLIB
#include glb_$(OS).mak
#OGLLIB = -lGLU -lGL
OGLLIB = -lGL


VPATH = $(OUTDIR):$(DIRSRC1)

CC = gcc


ifeq "$(OS)" "Linux32"
FTYP = so
CPFLG = -fPIC $(CPDEB) $(GUICP) $(GLBCP)
CPPFLG = -fPIC $(CPDEB)
LKFLG = -shared -export-dynamic
endif

ifeq "$(OS)" "Linux64"
FTYP = so
CPFLG = -fPIC $(CPDEB) $(GUICP) $(GLBCP)
CPPFLG = -fPIC $(CPDEB)
LKFLG = $(LKDEF) $(LKDEB) -shared
endif

#ifeq "$(OS)" "MSGCC32"
#FTYP = dll
#OUTDIR=../bin$(OS)
#LKFLG = -shared gCAD3D.lib
#endif

# -D_INTEL
# -Wl,-soname,$(DLLNAM)



#=====================================================================
default: $(OBJ1)
#	@echo "========================= Link gui-version:" $(VGUI)
#	@echo "active gui-version:" $(VGUI)
#	@echo "active system:" $(OS)
#	@echo "VPATH:" $(VPATH)
#	@echo "SRC1:" $(SRC1)
#	@echo "OBJ1:" $(OBJ1)
	@echo ====================== Link ======================
	cd $(OUTDIR) &&\
	$(CC) -o $(DLLNAM).$(FTYP) $(LKDEB) $(OBJ1)\
  $(GUILIB) $(OGLLIB) $(LKFLG)

#ifeq "$(DEB)" "2"
#	strip -o $(OUTDIR)/$(DLLNAM).$(FTYP) $(OUTDIR)/$(DLLNAM).$(FTYP)
#endif

#	if test $(OUTDIR) != $(DIRBIN); then mv -f $(OUTDIR)/$(DLLNAM).$(FTYP) $(DIRBIN)/.; fi



#=====================================================================
# Ausgabe der ObjectFileList (f xa_find ..)
objlst:
	@echo $(OBJ1)



#=====================================================================
# vi holt damit seine sourcefileliste
srclst:
	@echo $(SRC1) ../APP/tst*.c > srcFiles


#=====================================================================
# remove all gui-objects
remove:
	cd $(OUTDIR) && \
	rm -rf $(OBJGUI)


#=====================================================================
# ACHTUNG: case of filename muss genau stimmen, else xx.o Datei nicht gefunden
.c.o:
	@echo ====================== C $@ ========================
	$(CC) -c $(CPFLG) $<
#	if test $(@D) != "../xa"; then mv $(@F) $@; fi
	mv -f $(@F) $(OUTDIR)/.
	ctags -f ../APP/$(*F).tag $<


.cpp.o:
	@echo ================= Cpp $@ ========================
	$(CC) -c $(CPPFLG) $<
	mv -f $(@F) $(OUTDIR)/.
	ctags -f ../LIB_gui/$(*F).tag $<

# EOF



########################### EOF #######################
