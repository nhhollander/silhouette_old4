#!/bin/bash

set -e # Stop processing on error

printf "\033[1;32mDownloading SDL2 source\033[0m\n"
wget https://www.libsdl.org/release/SDL2-2.0.9.tar.gz

printf "\033[1;32mExtracting source\033[0m\n"
tar -xvf SDL2-2.0.9.tar.gz

printf "\033[1;32mBuilding\033[0m\n"
cd SDL2-2.0.9
./configure
make

printf "\033[1;32mInstalling\033[0m\n"
sudo make install

printf "\033[1;32mCleaning up\033[0m\n"
cd ../
rm SDL2-2.0.9.tar.gz
rm -rdf SDL2-2.0.9

printf "\033[1;32mDone\033[0m\n"