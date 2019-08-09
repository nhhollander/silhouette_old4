#!/bin/bash

# This file configures and launches the test executable with debug settings
# applied

export SE_DEBUG_USERDATA=./data/user
export SE_DEBUG_TEMPDATA=./data/temp
export SE_DEBUG_CACHEDATA=./data/cache
export SE_DEBUG_APPDATA=./data/appdata

export SE_APPNAME=se_test

# Make sure all directories are present
mkdir -p $SE_DEBUG_TEMPDATA
mkdir -p $SE_DEBUG_CACHEDATA

# Remove temporary files (because they can't be expected to survive)
if [ ! -z "$SE_DEBUG_TEMPDATA" ]; then
    echo "Clearing temporary debug files"
    rm -rdf $SE_DEBUG_TEMPDATA/*
fi

# Launch the program
./bin/se_test