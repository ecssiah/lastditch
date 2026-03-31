#!/usr/bin/env sh

set -e

BUILD_TYPE=${1:-Debug}
TARGET=${2:-game}

case "$TARGET" in
    game)
        EXECUTABLE=ld_game
        ;;
    edit)
        EXECUTABLE=ld_edit
        ;;
    *)
        echo "Unknown target: $TARGET"
        exit 1
        ;;
esac

echo "Configuring ($BUILD_TYPE)..."
cmake -S . -B build -DCMAKE_BUILD_TYPE=$BUILD_TYPE

echo "Building..."
cmake --build build --target $EXECUTABLE -j

echo "Tags..."
ctags -e -R .

echo "Running ($EXECUTABLE)..."
./build/$EXECUTABLE
