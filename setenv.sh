#!/bin/bash

# export a few mandatory environment variables
export TIGERBEETLE_CPPPATH="$(pwd)/contrib/tigerbeetle/src"
export TIGERBEETLE_LIBPATH="$(pwd)/contrib/tigerbeetle/src"
export LD_LIBRARY_PATH="$TIGERBEETLE_LIBPATH:$LD_LIBRARY_PATH"