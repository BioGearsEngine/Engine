#!/bin/sh

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

if [[ -d "./cmake/xcode/$2" ]]
then
    cd "cmake/xcode/$2"
else
    if [[ $1 == "clean" ]]
    then
        echo "Clean successful"
        exit 0
    else
        echo "Cmake directory does not exist, run ant cmake -Denv=xcode in BioGears/src to generate the make files"
        exit 1
    fi
fi

if [[ $2 == "debug" ]]
then
    xcodebuild -configuration Debug -scheme ALL_BUILD $1 OTHER_CPLUSPLUSFLAGS="-std=c++11 -stdlib=libc++"
elif [[ $2 == "release" ]]
then
    xcodebuild -configuration Release -scheme ALL_BUILD $1 OTHER_CPLUSPLUSFLAGS="-std=c++11 -stdlib=libc++"
fi

if [[ $? != 0 ]]
then
    echo "xcodebuild failed"
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
