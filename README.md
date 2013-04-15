#5C Hackathon Gameboy Project - "Room Full of Cats"

##Compiling
Room Full of Cats makes extensive use of the
[GBDK](http://gbdk.sourceforge.net) toolchain for Windows. To compile, add 
lcc to your path and run Make.bat. These tools have not been updated in a long
time, so there is minimal support online.

##Running the Game
Room Full of Cats runs best on the [no$gmb](http://nocash.emubase.de/gmb.htm) 
emulator. The game will also run on actual hardware using a Flash cartridge.

##Note about Windows
Since many of the tools for Game Boy development were created for Windows, this
project contains only a Windows-compatible Makefile. The lcc commands in the
Makefile will probably work on OS X or Linux but they have not been tested.
