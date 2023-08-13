#!/usr/bin/bash
glslc.exe -fshader-stage=frag  frag.glsl -o frag.spv
glslc.exe -fshader-stage=vert  vert.glsl -o vert.spv


