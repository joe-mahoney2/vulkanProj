#!/usr/bin/bash
cmake -S . -B ./out/build
cmake --build ./out/build --config Release
cmake --install ./out/build 
