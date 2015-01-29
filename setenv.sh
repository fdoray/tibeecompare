#!/bin/bash

# export a few mandatory environment variables
export TIGERBEETLE_CPPPATH="$(pwd)/contrib/tigerbeetle/src"
export TIGERBEETLE_LIBPATH="$(pwd)/contrib/tigerbeetle/src"
export LIBDELOREAN_CPPPATH="$(pwd)/contrib/tigerbeetle/contrib/libdelorean/include"
export LIBDELOREAN_LIBPATH="$(pwd)/contrib/tigerbeetle/contrib/libdelorean/src"
export BABELTRACE_CPPPATH="$(pwd)/contrib/tigerbeetle/contrib/babeltrace/include"
export BABELTRACE_LIBPATH="$(pwd)/contrib/tigerbeetle/contrib/babeltrace/lib/.libs"
export BABELTRACE_CTF_LIBPATH="$(pwd)/contrib/tigerbeetle/contrib/babeltrace/formats/ctf/.libs/"
export LEVELDB_INCLUDEPATH="$(pwd)/contrib/leveldb/include"
export LEVELDB_LIBPATH="$(pwd)/contrib/leveldb"
export LD_LIBRARY_PATH="$TIGERBEETLE_LIBPATH:$LIBDELOREAN_LIBPATH:$BABELTRACE_LIBPATH:$BABELTRACE_CTF_LIBPATH:$LEVELDB_LIBPATH:$LD_LIBRARY_PATH"