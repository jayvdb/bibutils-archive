
Compiling bibutils version 2.1

1.  Edit the makefile.  A number of configurations are present.  Pick
the useful one and comment out the others.  (You will probably want to
clear the POSTFIX variable as in the "# Linux mine" section.  This is
just to automatically make binaries with different names under different
architectures for the web site.)

2.  Type "make" at the command line.

3.  Edit the INSTALLDIR variable in the makefile and type "make install"
to copy the binaries to an appropriate directory.


