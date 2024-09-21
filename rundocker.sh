#!/bin/sh

container_name="$1"
if [ -z "$1" ]; then
    container_name="compilers_cw_machine"
fi

docker rm "$container_name"
docker run -it -v "`pwd`:`pwd`" -w "`pwd`" --name "$container_name" compilers_cw
