#!/usr/bin/bash
"$VK_SDK_PATH"/bin/glslc -fshader-stage=frag  shaders/frag.glsl -o shaders/frag.spv
"$VK_SDK_PATH"/bin/glslc -fshader-stage=vert  shaders/vert.glsl -o shaders/vert.spv
echo "Shaders compiled"


