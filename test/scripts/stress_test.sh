#!/usr/bin/env bash

make_fs(){
    rm -Rf ./virtualfs* ./vfs*
    local fs_nb=$1
    local pause=$2
    for (( c=0; c<=$fs_nb; c++ ))
    do
        dd if=/dev/zero of=./virtualfs${c} bs=1M count=1
        mkfs.ext4 -F ./virtualfs${c}
        mkdir ./vfs${c}
        mount -t ext4 -o loop ./virtualfs${c} ./vfs${c}
        rm -Rf ./vfs${c}/test && dd if=/dev/zero of=./vfs${c}/test bs=1024 count=800
        sleep ${pause}
        curl -i '127.0.0.1:1531/'
    done
    umount ./vfs*
    rm -Rf ./virtualfs* ./vfs*
}

cd /tmp
make_fs $1 $2
cd -