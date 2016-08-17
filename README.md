STORED
======

## Motivation
For fun.
Autotools and glibc/Linux as development platform, for webdeveloper:D

## Contents
This little project puts syslog msg. when any mount point has less than 30%  space.
It has also tiny builtin web server so you can get your warnings in restfull manner.

## Compilile and run
```
$ ./configure && make
# make install
#systemctl enable stored.service
#systemctl start stored.service
# ./test/create_filled_fs.sh <num fs> <pause sec.>`.
```