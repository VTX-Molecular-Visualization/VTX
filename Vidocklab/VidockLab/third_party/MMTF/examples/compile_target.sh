#!/bin/bash

if [ "$2" == "" ]; then
  echo "USAGE: compile_target.sh <MSGPACK_INCLUDE_PATH> <MAIN_FILE>"
  echo "-> for MAIN_FILE = test.cpp, this produces executable test.exe"
  exit 1
fi

MAIN_FILENAME="${2%.*}"

g++ -I$1 -I"../include" -O2 -o $MAIN_FILENAME.exe $MAIN_FILENAME.cpp
