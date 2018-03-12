# Inkscape+SAGE build on Linux.
# I had to do many modification to the SAGE code to compile it with Linux.
# It compiles but is not stable. I NEED SOME HELP HERE!

sudo apt-get remove inkscape
sudo apt-get install bzr
sudo apt-get build-dep inkscape

# this will bring release 0.48+devel r13560
bzr co --lightweight lp:inkscape trunk -r 12830

# this will bring the latest development version
# I don't know how to merge it with the sage/xsac extensions.
# bzr co --lightweight lp:inkscape

cp trunk/src/ui/dialog/object-properties.h trunk/src/ui/dialog/object-properties.h.bak
cp trunk/src/ui/dialog/object-properties.cpp trunk/src/ui/dialog/object-properties.h.cpp

cp Makefile_insert isac-menu.cpp Json.cpp object-properties.cpp isac-menu.h  Json.h object-properties.h ../trunk/src/ui/dialog/

cd trunk
./autogen.sh
./configure
make 
sudo make install

