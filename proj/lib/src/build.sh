#!/usr/bin/bash
#build this shit here son
dir=${BASH_SOURCE[0]}

base=${dir::-8}
src=$base
out=$base\out/build

cmake -S $src -B $out
cmake --build $out --config Release
cmake --install $out 