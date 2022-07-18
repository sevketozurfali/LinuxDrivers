#!/bin/bash

makefile=$1

if [ $makefile == "" ]
then
    make -C /lib/modules/$(uname -r)/build/ M=$makefile modules

else
    make -C /lib/modules/$(uname -r)/build/ M=$PWD modules
fi