#!/bin/bash
BUILD_TYPE="$1"
COMPILER="$2"
TARGET="$3"

if [[ -z "$BUILD_TYPE" ]]; then
  echo "USAGE: ARG 1 -> BUILD TYPE (Release, Debug, Etc)"
  exit 1
fi

if [[ -z "$COMPILER" ]]; then
  echo "USAGE: ARG 2 -> COMPILER (gcc, clang, etc)"
  exit 1
fi

if [[ -z "$TARGET" ]]; then
  echo "USAGE: ARG 3 -> TARGET (Windows, Linux)"
  exit 1
fi

cmake \
  -S . \
  -DCMAKE_SYSTEM_NAME="$TARGET" \
  -G "Ninja" \
  -B build \
  -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
  -DCMAKE_C_COMPILER="$COMPILER"

cmake --build build
