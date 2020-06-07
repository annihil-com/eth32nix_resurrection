# Needed packages:

elfutils(libelf) scons base-devel(binutils, gcc, gcc-libs) pkg-config

# Compiling

1. $ cd to precompiled folder
2. $ sh compile.sh

That's it, the script "compile.sh" will generate and move all the required files to the corresponding folders:

libresurrection.so to "eth32nix-resurrection/precompiled/"
resurrection.pk3 to "~/.etwolf/etmain"
cgame.mp.i386.so to "eth32nix-resurrection/precompiled/dll"

# Running

Edit the script run.sh, located in: "eth32nix-resurrection/precompiled/run.sh"

export ETH32_ET="/path/to/enemy-territory"
export ETH32_DIR="/path/to/eth32/precompiled"

ETH32_ET is the path to your enemy territory installation (where et.x86 is)
ETH32_DIR is the "precompiled" folder inside eth32nix-resurrection, note there is not a slash (/) at the end of the dir

Save it with your changes and execute it:

$ sh run.sh