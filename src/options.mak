#

# Version GTK; gtk2 or gtk3
#VGUI = gtk2
VGUI = gtk3



# do not modify ....
BITS := $(shell getconf LONG_BIT)

ifeq "$(BITS)" "32"
OS := Linux32
else
OS := Linux64
endif

OUTDIR := ../../bin$(OS)

# eof
