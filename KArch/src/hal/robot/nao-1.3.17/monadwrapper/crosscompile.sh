#!/bin/bash
##
## crosscompile.sh
## Login : <ctaf@localhost.localdomain>
## Started on  Tue Oct  7 12:24:21 2008 Cedric GESTES
## $Id$
##
## Author(s):
##  - Cedric GESTES <gestes@aldebaran-robotics.com>
##
## Copyright (C) 2008 Aldebaran Robotics
##

#
# Why this script?
# because cmake needs CC and CXX defined to cross-compile well..because it checks
# the compiler at the first cmake run.
# When configured a build folder can't be changed to use a different compiler
# CFLAGS need to be defined here too because cmake tests the compiler with a little
# program and without sysroot this cant work.

# cross compilation is tedious with cmake...

usage() {
  echo "usage:"
  echo "crosscompile <crosstoolchainfolder> <sourcefolder> [options]"
  echo "configure your build folder for cross-compilation"
  echo ""
  echo "for example:"
  echo "  mkdir build"
  echo "  cd build"
  echo "  ../crosscompile ~/bin/ctc .. <CMAKE ARG>"
  exit 1
}

export ROOT_BUILD_DIR=$(pwd)
export CTC_DIR=$(readlink -f "$1")
export ROOT_SOURCE_DIR=$(readlink -f "$2")

if [ $# -lt 2 ]; then
  usage
fi

shift 2

export BUILD_ARG="$@"

export PATH_CC="${CTC_DIR}/cross/bin"

export CXX=${PATH_CC}/i486-linux-g++
export CC=${PATH_CC}/i486-linux-gcc
export LD=${PATH_CC}/i486-linux-ld
export AR=${PATH_CC}/i486-linux-ar
export RANLIB=${PATH_CC}/i486-linux-ranlib

export CPPFLAGS="--sysroot ${CTC_DIR}/staging/i486-linux/ \
-I${CTC_DIR}/staging/i486-linux/usr/include/ \
-I${CTC_DIR}/cross/lib/gcc/i486-linux/4.2.2/include/ \
-I${CTC_DIR}/staging/i486-linux/usr/include/c++/ \
-I${CTC_DIR}/staging/i486-linux/usr/include/c++/i486-linux/"

export CFLAGS="--sysroot ${CTC_DIR}/staging/i486-linux/ \
-I${CTC_DIR}/staging/i486-linux/usr/include/ \
-I${CTC_DIR}/cross/lib/gcc/i486-linux/4.2.2/include/ \
-I${CTC_DIR}/staging/i486-linux/usr/include/c++/ \
-I${CTC_DIR}/staging/i486-linux/usr/include/c++/i486-linux/"

export LDFLAGS="--sysroot ${CTC_DIR}/staging/i486-linux/ \
-lgcc -L${CTC_DIR}/cross/i486-linux/lib/ -lc -lstdc++ -ldl"


#check if we build in a build folder
if [ "$ROOT_SOURCE_DIR" = "$ROOT_BUILD_DIR" ] ; then
  tput setf 4
  echo "please use a build directory dont build in your root source folder"
  echo "> mkdir build"
  echo "> cd build"
  echo "> ../crosscompile.sh <ctc folder>"
  tput sgr0
  exit 42
fi

#check if the root source dir is clean
if [ -f $ROOT_SOURCE_DIR/CMakeCache.txt ] ; then
  tput setf 4
  echo "error your root source folder has been configured (you ran cmake .)"
  echo "you should not build in your source folder, but in a build directory"
  echo "please restart from scratch...do a fresh new checkout"
  tput sgr0
  exit 43
fi

if [ -f $ROOT_BUILD_DIR/CMakeCache.txt ] ; then
  tput setf 4
  echo "error your build folder has been already configured"
  echo "please cleanup your build directory before calling crosscompile.sh"
  tput sgr0
  exit 44
fi

if ! [ -d $CTC_DIR/cross ] || ! [ -d $CTC_DIR/staging ] ; then
  tput setf 4
  echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
  echo "Cross tool chain not found in : "
  echo "$CTC_DIR"
  echo "Please check the path and try again."
  echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
  tput sgr0
  echo ""
  usage
  exit 45
fi


tput setf 4
echo "configuring naoqi for cross-compilation"

tput setf 3
echo "cross tool chain folder: $CTC_DIR"
echo "naoqi folder           : $ROOT_SOURCE_DIR"
echo "build folder           : $ROOT_BUILD_DIR"
echo "build arguments        : $BUILD_ARG"
echo ""
tput sgr0

cmake -DOE_CROSS_BUILD:INTERNAL=ON -DOE_CROSS_DIR:INTERNAL=$CTC_DIR $@ $ROOT_SOURCE_DIR
