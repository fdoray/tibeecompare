#!/bin/bash

# this script will initialize the submodules of the contrib system,
# update them and build them.

# init/update Git submodules
pushd ..
git submodule init
git submodule update
popd

# build tigerbeetle
pushd tigerbeetle
./bootstrap.sh
source setenv.sh
scons
popd
