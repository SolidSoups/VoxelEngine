#!/bin/bash

set -e

if [ ! -d "./build" ]; then
  mkdir build
fi

cmake -B build -S . -G Ninja -Wno-dev -Wno-deprecated -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

ninja -C build
