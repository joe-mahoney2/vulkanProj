#!/usr/bin/bash
# This script builds a CMake project
# Get the directory of the script
dir=$(dirname "${BASH_SOURCE[0]}")
# Set the source directory to the parent directory of the script directory
src=$(dirname "$dir")/src
# Set the output directory to the script directory + /out/build
out="$dir/out/build"

# Create the output directory if it does not exist
if [ ! -d "$out" ]; then
  mkdir -p "$out"
fi

# Run the CMake configure command
cmake -S "$src" -B "$out"

# Check if the CMake configure command succeeded
if [ $? -eq 0 ]; then
  # Run the CMake build command
  cmake --build "$out" --config Release
  # Run the CMake install command
  cmake --install "$out"
else
  # Print an error message if the CMake configure command failed
  echo "CMake configuration failed"
fi