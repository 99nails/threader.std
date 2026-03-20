#!/bin/bash

START_LOCATION=$(pwd)
echo "Started from: ${START_LOCATION}"

buildType="-DCMAKE_BUILD_TYPE=Debug"

for parameter in "$@"
do
    if [[ "$parameter" == "release" ]]; then
        echo "Using RELEASE configuration!"
        buildType="-DCMAKE_BUILD_TYPE=Release"
    fi
    if [[ "$parameter" == "debug" ]]; then
        echo "Using DEBUG configuration!"
        buildType="-DCMAKE_BUILD_TYPE=Debug"
    fi
done

#exit

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
cmake .. "$buildType" || terminate

echo "Making threader.............................................................................."
make -j`nproc` || terminate

echo "Building deb packets........................................................................."
cpack -G DEB || terminate

echo "Installing threader.........................................................................."
sudo cmake --install . || terminate

terminate

