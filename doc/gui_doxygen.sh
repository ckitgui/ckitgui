#!/bin/sh
# UPDATE files src/ckitgui_version and src/ckitgui_date
# ReCreate documentation:   ./gui_doxygen.sh
#   active dir must be doc/ or src/
#




# version, date, project-name
version=`cat ../src/ckitgui_version`
date=`cat ../src/ckitgui_date`
doxy_prj_nam="C-KIT-GUI-GTK $version"
export doxy_prj_nam


# path from doxygen-InputfileDirectory -> sourceBasedirectory
# start create doc here ..
srcBase="../src/APP"

# path from srcBase -> doxygen-InputfileDirectory
doxBase="../../doc/gui_doxygen"

# path from srcBase -> doxygen-outputDirectory
doxy_path_out="../../doc/gui"
export doxy_path_out

# path from doxygen-outputDirectory -> srcBase
dox_path_src="../../src"
export dox_path_src

doxy_path_base=`pwd`
export doxy_path_base

# sourcedirectories
srcDirs="\
 ../APP\
 ../gui\
 ../gui_gtk2\
 ../gui_gtk3\
 ../ut\
"
export srcDirs


# all dox-files; this makes the main-menu.
# Do not change first line (sourcedirectories)
files="${srcDirs}\
 ${doxBase}/mainpage.dox\
 ${doxBase}/Introduction.dox\
 ${doxBase}/Sourcefiles.dox\
 ${doxBase}/Download.dox\
 ${doxBase}/TODO.dox\
"
export files


############# work ..
cd $srcBase


# update version & date in mainpage.dox
cat ${doxBase}/mainpage
cat ${doxBase}/mainpage | sed "s/<VERSION>/$version/"  | sed "s/<DATE>/$date/" > ${doxBase}/mainpage.dox
#exit


# create file srcFiles
make -f c-kit-gui-gtk.mak srclst
#cat srcFiles


# create Sourcefiles.dox
. ${doxBase}/doxy_help1.sh
mv -f Sourcefiles.dox ${doxBase}/.
#exit


# clear outDir, but keep .gitignore
rm -rf ${doxy_path_out}/*


# copy image
cp ${doxBase}/*.jpg ${doxy_path_out}/.


# start doxygen
# doxygen-input: $doxy_prj_nam $doxy_path $files and file Doxyfile
doxygen ${doxBase}/Doxyfile


# display new docu
xdg-open ${doxy_path_out}/index.htm

exit
# EOF
