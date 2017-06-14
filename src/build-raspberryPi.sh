#!/bin/bash

if [[ $1 != "build" && $1 != "clean" ]]
then
    echo "usage: [build^|clean] [debug^|release]"
    exit 1
fi

if [[ $2 != "debug" && $2 != "release" ]]
then
    echo "usage: [build^|clean] [debug^|release]"
    exit 1
fi

if [[ -d "./cmake/raspberryPi/$2" ]]
then
    cd "cmake/raspberryPi/$2"
else
    if [[ $1 == "clean" ]]
    then
        echo "Clean successful"
        exit 0
    else
        echo "Cmake directory does not exist, run ant cmake -Denv=raspberryPi in BioGears/src to generate the make files"
        exit 1
    fi
fi

make

if [[ $? != 0 ]]
then
    echo "make failed"
    exit 1
else
    if [[ $1 == "clean" ]]
    then
        echo "Clean successful"
        exit 0
    elif [[ $1 == "build" ]]
    then
        echo "Build successful"
        exit 0
    fi
fi
