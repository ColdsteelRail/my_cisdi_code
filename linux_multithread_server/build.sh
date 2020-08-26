#!/bin/sh

set -x # 以下的命令生成log打印

SOURCE_DIR=`pwd`
BUILD_DIR=${BUILD_DIR:-../../build_my_code}
BUILD_TYPE=${BUILD_TYPE:-release}
INSTALL_DIR=${INSTALL_DIR:-../${BUILD_TYPE}-install-cpp11}
CXX=${CXX:-g++} # C++编译器选项

#ln -sf $BUILD_DIR/$BUILD_TYPE-cpp11/compile_commands.json

mkdir -p $BUILD_DIR/$BUILD_TYPE-cpp11 \
  && cd $BUILD_DIR/$BUILD_TYPE-cpp11 \
  && cmake \ # generate the CMakeFile, CMameCache.txt, cmake_install.cmake, Makefile
           -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
           -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
           -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
           $SOURCE_DIR \ # 执行pwd
  && make $* 
  #cmake  -DCMAKE_BUILD_TYPE=release \ 
  #       -DCMAKE_INSTALL_PREFIX=../release-install-cpp11 \
  #       -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  #       /home/dcn-space/myCodeSpace/my_cisdi_code/linux_multithread_server

# Use the following command to run all the unit tests
# at the dir $BUILD_DIR/$BUILD_TYPE :
# CTEST_OUTPUT_ON_FAILURE=TRUE make test

# cd $SOURCE_DIR && doxygen

