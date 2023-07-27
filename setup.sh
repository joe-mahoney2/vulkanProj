#!/usr/bin/bash
git submodule update --init

cmake -S ./modules/glfw -B ./modules/glfw/out/build
cmake --build ./modules/glfw/out/build
cmake -S ./modules/vk-bootstrap -B ./modules/vk-bootstrap/out/build
cmake --build ./modules/vk-bootstrap/out/build