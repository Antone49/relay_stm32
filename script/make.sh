#!/bin/bash

currentDirectory=$(realpath `dirname $0`)

# Remove build directory si argument clean
if [ -d "$currentDirectory/../src/_build" ]; then
    if [ "$1" == "clean" ]; then
        docker run --rm -it -e RUN_AS=root -e UID=root -e GID=root -v .:/root/laser_level:rw laser_level sh -c "rm -rf src/_build"
        docker run --rm -it -e RUN_AS=root -e UID=root -e GID=root -v .:/root/laser_level:rw laser_level sh -c "rm bin/*"
    fi
fi

# Generation cmake si le dossier build n'est pas present
if [ ! -d "$currentDirectory/../src/_build" ]; then
    docker run --rm -it -e RUN_AS=root -e UID=root -e GID=root -v .:/root/laser_level:rw laser_level sh -c "cd src && cmake . -B _build"
fi

docker run --rm -it -e RUN_AS=root -e UID=root -e GID=root -v .:/root/laser_level:rw laser_level sh -c "cd src && cmake --build _build --target install -j16"