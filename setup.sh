#!/usr/bin/bash
#
# SETUP GLFW
git submodule update --init
cmake	-S ./modules/glfw \
	-DCMAKE_INSTALL_PREFIX=./modules \
	-DCMAKE_INSTALL_INCLUDEDIR=./include \
	-DCMAKE_INSTALL_LIBDIR=./lib \
	-B ./modules/glfw/out/build

cmake --build ./modules/glfw/out/build --config "Release"
cmake --install ./modules/glfw/out/build --config "Release"
#
# SETUP VK-BOOTSTRAP
cmake 	-S ./modules/vk-bootstrap \
	-DCMAKE_INSTALL_PREFIX=./modules \
	-DCMAKE_INSTALL_INCLUDEDIR=./include \
	-DCMAKE_INSTALL_LIBDIR=./lib \
	-B ./modules/vk-bootstrap/out/build \

cmake --build ./modules/vk-bootstrap/out/build --config "Release"
cmake --install ./modules/vk-bootstrap/out/build --config "Release"
