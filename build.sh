#!/bin/bash

test -e "tools/appmaker/bin/appmaker"
if [ $? -ne 0 ]; then
	pushd .
	cd tools/appmaker/
	make clean
	make
	popd
fi

test -e "tools/sysmaker/bin/sysmaker"
if [ $? -ne 0 ]; then
	pushd .
	cd tools/sysmaker/
	make clean
	make
	popd
fi

test -e "tools/diskmaker/bin/diskmaker"
if [ $? -ne 0 ]; then
	pushd .
	cd tools/diskmaker/
	make clean
	make
	popd
fi

make clean
make
