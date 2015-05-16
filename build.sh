#!/bin/bash

test -e "tools/appmaker/bin/appmaker"
if [ $? -ne 0 ]; then
	pushd .
	cd tools/appmaker/
	make Clear
	make
	popd
fi

test -e "tools/sysmaker/bin/sysmaker"
if [ $? -ne 0 ]; then
	pushd .
	cd tools/sysmaker/
	make Clear
	make
	popd
fi

test -e "tools/diskmaker/bin/diskmaker"
if [ $? -ne 0 ]; then
	pushd .
	cd tools/diskmaker/
	make Clear
	make
	popd
fi

make Clear
make
