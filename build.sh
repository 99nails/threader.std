#!/bin/bash

START_LOCATION=$(pwd)
echo "Started from: ${START_LOCATION}"

function terminate() {
    echo "Returning to start location.................................................................."
    cd ${START_LOCATION}
    exit
}

if [[ ! -d ./build ]]; then
    echo "Creating build folder..."
    mkdir build || terminate
fi

echo "Change folder to ./build....................................................................."
cd build

echo "Running cmake................................................................................"
cmake .. || terminate

echo "Making threader.............................................................................."
make -j`nproc` || terminate

echo "Installing threader.........................................................................."
sudo cmake --install . || terminate

terminate

