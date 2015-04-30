#!/bin/bash

# this script will initialize the submodules of the contrib system,
# update them and build them.

# Get sudo access.
sudo echo Starting Installation...

# Exit on error.
set -e

# install useful tools
sudo apt-get install -qq build-essential unzip wget

# install lttng-tools dependencies
sudo apt-get install -qq libpopt-dev uuid-dev libxml2-dev
sudo apt-get install -qq perl python3 python3-dev swig2.0
sudo apt-get install -qq module-init-tools

# install lttng-ust dependencies
sudo apt-get install -qq libtool autoconf

# install babeltrace dependencies
sudo apt-get install -qq libc6-dev libglib2.0-0 libglib2.0-dev flex bison

rm -rf lttng
mkdir lttng
pushd lttng

# download sources
wget -O userspace-rcu.tar.gz https://github.com/urcu/userspace-rcu/archive/v0.8.6.tar.gz
wget -O lttng-ust.tar.gz https://github.com/lttng/lttng-ust/archive/v2.6.0.tar.gz
wget -O babeltrace.tar.gz https://github.com/efficios/babeltrace/archive/v1.2.4.tar.gz
wget -O lttng-tools.tar.gz https://github.com/lttng/lttng-tools/archive/v2.6.0.tar.gz
wget -O lttng-modules.tar.gz https://github.com/fdoray/lttng-modules/archive/version2.6.0-addons-tp.tar.gz
wget -O libunwind.tar.gz https://github.com/fdoray/libunwind/archive/cfa_ret_only.tar.gz
wget -O lttng-profile.tar.gz https://github.com/fdoray/lttng-profile/archive/master.tar.gz
wget -O lttng-profile-modules.tar.gz https://github.com/fdoray/lttng-profile-modules/archive/master.tar.gz

tar -xf userspace-rcu.tar.gz
tar -xf lttng-ust.tar.gz
tar -xf babeltrace.tar.gz
tar -xf lttng-tools.tar.gz
tar -xf lttng-modules.tar.gz
tar -xf libunwind.tar.gz
tar -xf lttng-profile.tar.gz
tar -xf lttng-profile-modules.tar.gz

# enable python bindings for lttng
export PYTHON="python3"
export PYTHON_CONFIG="/usr/bin/python3-config"

# install userspace-rcu
pushd userspace-rcu-0.8.6
./bootstrap
./configure
make -j4
sudo make install
sudo ldconfig
popd

# install lttng-ust
pushd lttng-ust-2.6.0
./bootstrap
./configure
make -j4
sudo make install
sudo ldconfig
popd

# install babeltrace
pushd babeltrace-1.2.4
./bootstrap
./configure --enable-python-bindings
make
sudo make install
sudo ldconfig
popd

# install lttng-tools
pushd lttng-tools-2.6.0
./bootstrap
./configure --enable-python-bindings
make -j4
sudo make install
sudo ldconfig
popd

# install lttng-modules with addons
pushd lttng-modules-version2.6.0-addons-tp
make -j4
sudo make modules_install
sudo mkdir -p /usr/local/bin/
sudo install control-addons.sh /usr/local/bin/
sudo sh -c 'echo "search extra updates ubuntu built-in" > /etc/depmod.d/ubuntu.conf'
sudo depmod -a
popd

# install libunwind
pushd libunwind-cfa_ret_only
./autogen.sh
./configure --enable-block-signals=false
make -j4
sudo make install
popd

# install lttng-profile
pushd lttng-profile-master
./bootstrap
./configure
make -j4
sudo make install
sudo ldconfig
popd

# install lttng-profile-modules
pushd lttng-profile-modules-master
make -j4
sudo make modules_install
sudo depmod -a
sudo mkdir -p /usr/local/bin/
sudo install lttngprofile-start.sh /usr/local/bin/
popd

popd

echo "end of script"
echo "rerun the script in case the kernel is updated"
echo "reboot to complete setup"
