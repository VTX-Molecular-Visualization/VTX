#!/bin/bash

if [ "$3" == "" ]; then
  echo "USAGE: compile_and_run.sh <MSGPACK_INCLUDE_PATH> <MAIN_FILE> <MMTF_FILE>"
  echo "-> for MAIN_FILE = test.cpp, we compile and run test.exe <MMTF_FILE>"
  exit 1
fi

MAIN_FILENAME="${2%.*}"
./compile_target.sh $1 $MAIN_FILENAME.cpp &&
./$MAIN_FILENAME.exe $3
