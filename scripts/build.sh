#!/usr/bin/env sh

set -e

BUILD_TYPE=${1:-Debug}

EXECUTABLE=lastditch

if [ "$2" = "clean" ]; then
    echo "Cleaning build directory..."
    rm -rf build
fi

echo "Configuring ($BUILD_TYPE)..."
cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=$BUILD_TYPE

echo "Building..."
cmake --build build --target $EXECUTABLE -j

echo "Tags..."
ctags -e -R .

echo "Running ($EXECUTABLE)..."
./build/$EXECUTABLE
