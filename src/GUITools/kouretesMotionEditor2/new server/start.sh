#!/bin/bash
SDK_DIR=$AL_DIR
export LD_LIBRARY_PATH="${SDK_DIR}/lib"
export PYTHONHOME="${SDK_DIR}"
export PYTHONPATH="${SDK_DIR}/lib"
echo $@
python2.6 $@
