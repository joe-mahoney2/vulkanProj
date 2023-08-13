#!/usr/bin/bash
"$VK_SDK_PATH"/Bin/glslc.exe -fshader-stage=frag  frag.glsl -o frag.spv
"$VK_SDK_PATH"/Bin/glslc.exe -fshader-stage=vert  vert.glsl -o vert.spv
echo "Shaders compiled"


