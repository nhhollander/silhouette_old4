#!/bin/bash

set -e # Stop processing on error

printf "\033[1;32mPreparing\033[0m\n"
rm -rdf glew

printf "\033[1;32mDownloading glew\033[0m\n"
git clone https://github.com/nigels-com/glew.git glew

printf "\033[1;32mBuilding\033[0m\n"
cd glew
cd auto
make
cd ../
make

printf "\033[1;32mInstalling\033[0m\n"
sudo make install
sudo cp /usr/lib64/libGLEW.* /usr/lib/x86_64-linux-gnu/

printf "\033[1;32mCleaning up\033[0m\n"
cd ../
rm -rdf glew

printf "\033[1;32mDone\033[0m\n"