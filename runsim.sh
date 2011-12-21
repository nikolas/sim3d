#!/bin/zsh

if [ "$2" != "nobuild" ]; then
  if [ ! -d build ]; then
    mkdir build
  fi

  cd build
  if [ ! -f Makefile ]; then
    cmake ..
  fi

  make
  cd ..
fi

cd build/FOSSSim
./FOSSSim -s $1
