# Link & run:
#   make run -f tst_gui1.mak
# No Menu on Ubuntu: export UBUNTU_MENUPROXY=
# export LD_LIBRARY_PATH=../binLinux32 && gdb ../binLinux32/a.out


PRJNAM = tst_gui1

SRC1 =\
 tst_gui1.c\
 tst_gui_gl1.c


include ../APP/tst__.mak

# eof
