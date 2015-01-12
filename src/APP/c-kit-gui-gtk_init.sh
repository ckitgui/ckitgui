#!/bin/sh
# change gui-version VGUI to gtk2 or gtk3

zenity --info --text="set VGUI to gtk2 or gtk3"

vi ../options.mak

make remove -f c-kit-gui-gtk.mak

make -f c-kit-gui-gtk.mak

# eof
