#!/usr/bin/bash
#build this shit here son
dir=${BASH_SOURCE[0]}

base=${dir%/*} # this expression removes the last component of the path
echo $base
src=$base
out=$base\out/build

cmake -S $src -B $out
cmake --build $out --config Release
cmake --install $out 