#!/usr/bin/bash
# dir=`git rev-parse --show-toplevel`
# dir+=`git rev-parse --show-prefix`
# echo $dir

cmake -S . -B ./out/build
cmake --build ./out/build --config Release
