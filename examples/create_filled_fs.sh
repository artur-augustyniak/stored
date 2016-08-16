#!/usr/bin/env bash

cd /tmp
curl -i '127.0.0.1:8081/'
rm -Rf ./virtualfs ./vfs
dd if=/dev/zero of=./virtualfs bs=1024 count=30720
mkfs.ext4 -F ./virtualfs
mkdir ./vfs
mount -t ext4 -o loop ./virtualfs ./vfs
rm -Rf ./vfs/test && dd if=/dev/zero of=./vfs/test bs=1M count=22
curl -i '127.0.0.1:8081/'
#rm -Rf ./vfs/test && dd if=/dev/zero of=./vfs/test bs=1M count=19
#sleep 5
#curl -i '127.0.0.1:8081/'
#umount ./vfs
#curl -i '127.0.0.1:8081/'
#rm -f ./virtualfs
#cd -
