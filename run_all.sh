#!/usr/bin/env bash

# If project not ready, generate cmake file.
if [[ ! -d build ]]; then
    mkdir -p build
    cd build
    cmake ..
    cd ..
fi

rm bin/Proj
# Build project.
cd build
make -j
cd ..

# Run all testcases. 
# You can comment some lines to disable the run of specific examples.
mkdir -p output
# bin/Proj testcases/scene01_basic.txt output/scene01.bmp
# bin/Proj testcases/scene02_cube.txt output/scene02.bmp
# bin/Proj testcases/scene03_sphere.txt output/scene03.bmp
# bin/Proj testcases/scene04_axes.txt output/scene04.bmp
# bin/Proj testcases/scene05_bunny_200.txt output/scene05.bmp
# bin/Proj testcases/scene06_bunny_1k.txt output/scene06.bmp
# bin/Proj testcases/scene07_shine.txt output/scene07.bmp
# bin/Proj testcases/scene08_core.txt output/scene08.bmp
# bin/Proj testcases/scene09_norm.txt output/scene09.bmp
# bin/Proj testcases/scene11_bunny_ffd.txt output/scene11.bmp
# bin/Proj testcases/scene12_cylinder_ffd.txt output/scene12.bmp
# bin/Proj testcases/scene13_fish_ffd.txt output/scene13.bmp
bin/Proj testcases/scene.txt output/scene.bmp ppm 100 0.2 1.05 3