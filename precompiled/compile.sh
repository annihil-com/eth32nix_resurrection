#!/bin/bash

cd ../source
scons
scons pk3
mv ../precompiled/resurrection.pk3 ~/.etwolf/etmain

cd client
scons
mv cgame.mp.i386.so ../../precompiled/dll
