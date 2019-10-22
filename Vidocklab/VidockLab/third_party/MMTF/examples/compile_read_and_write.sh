#!/bin/bash

if [ "$3" == "" ]; then
  echo "USAGE: compile_read_and_write.sh <MSGPACK_INCLUDE_PATH> <MMTF_FILE_IN> <MMTF_FILE_OUT>"
  echo "-> read <MMTF_FILE_IN> and write as <MMTF_FILE_OUT>"
  exit 1
fi

./compile_target.sh $1 read_and_write.cpp &&
./read_and_write.exe $2 $3
