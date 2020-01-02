#!/usr/bin/env bash

# If project not ready, generate cmake file.
if [[ ! -d build ]]; then
    mkdir -p build
    cd build
    cmake ..
    cd ..
fi

rm bin/Proj
cd build
make -j
cd ..

mkdir -p output
bin/Proj testcases/scene.txt output/scene.bmp ppm 100 0.2 1.05 3