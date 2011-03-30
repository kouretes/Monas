#!/bin/bash


mkdir $1

sed -e "s/THEALSP/$1/g" AlSpAction_template.h > ./$1/$1.h
sed -e "s/THEALSP/$1/g" AlSpAction_template.cpp > ./$1/$1.cpp
sed -e "s/THEALSP/$1/g" AlSpAction_template.cmake > ./$1/CMakeLists.txt