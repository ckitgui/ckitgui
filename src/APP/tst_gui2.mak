# Link & run:
#   make run -f tst_gui2.mak
# No Menu on Ubuntu: export UBUNTU_MENUPROXY=
# export LD_LIBRARY_PATH=../binLinux32 && gdb ../binLinux32/a.out


PRJNAM = tst_gui2

SRC1 =\
 tst_gui2.c\
 tst_gui_gl2.c\
 trackball.c\
 surf2.c

include ../APP/tst__.mak

# eof
