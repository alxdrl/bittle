Bittle
======

Bittle is a simplistic Qt program that open an arbitrary file and then display
the binary data as a 1-bit monochrome bitmap.

This was inspired by a tool that existed on Amiga: you could warm start the
computer, launch the program and have a look at de content of memory. The main
usage of it was to look for graphics data from the program (read game) that was
running just before reboot.

Features
--------

- you can vary the stride, ie. the scanline width (in bytes) used to render the
data.
- you can move the display offset with the scrollbar
- you can vary the height of the display block

Build
-----

The source tree is a Qt Creator project and should build out of the box.

With qmake under MacOs X :

    #> mkdir <build dir>
    #> cd <build dir>
    #> qmake '<path to project>/bittle.pro' -r -spec macx-g++40 CONFIG+=x86_64 CONFIG+=declarative_debug
    #> make

Implementation
--------------

The file is memory mapped. 
Strips of 'stride * height' bytes are read from start offset then rendered as
QImage that in turn is drawn on a 'QPixmap'. The viewport (ie. the 'QPixmap'
surface) is then filled, left to right, top to bottom, until there no space
left.

