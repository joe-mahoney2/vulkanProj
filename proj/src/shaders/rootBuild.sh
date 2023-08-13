#!/usr/bin/bash
glslc.exe -fshader-stage=frag  shaders/frag.glsl -o shaders/frag.spv
glslc.exe -fshader-stage=vert  shaders/vert.glsl -o shaders/vert.spv
echo "Shaders compiled"


