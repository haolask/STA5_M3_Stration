#!/bin/bash -
#=============================================================================
#          FILE: build-ninja.sh
#
#         USAGE: ./build-ninja.sh
#
#   DESCRIPTION: Build xloaders with cmake invocation and use Ninja
#
#        AUTHOR: Philippe LANGLAIS
#  ORGANIZATION: ST
#=============================================================================

# add by fangyunmeng for env config
export PATH="$PWD/../gcc-arm-none-eabi-5_2-2015q4/bin:$PATH"
export STA_MEM_MAP_DIR=$PWD/../sta_mem_map


set -o nounset     # Treat unset variables as an error

if ! test -d build; then
	mkdir build
fi

if [ ! -d $STA_MEM_MAP_DIR ]
then
	echo "$STA_MEM_MAP_DIR is not a valid directory"
	exit 1
fi

cd build
cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=cmake/Toolchain-gcc-arm-none-eabi.cmake -DSTA_MEM_MAP_DIR=$STA_MEM_MAP_DIR ..
ninja
