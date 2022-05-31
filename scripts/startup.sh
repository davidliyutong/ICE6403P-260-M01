#!/bin/bash
VERSION=3.32

sudo apt-get install libgl1-mesa-dev

git submodule update --init --recursive
cd ns-3-allinone
python3 download.py -n ns-$VERSION
# ./build.py --enable-examples --enable-tests
cd ns-$VERSION
./waf configure --enable-examples --enable-tests
./waf
