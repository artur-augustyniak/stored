STORED
======

## Motivation
For fun.
Cmake and glibc/Linux as development platform, for webdeveloper:D

## Contents
This little project puts syslog msg. when any mount point has less than n%  space.
It has also tiny builtin web server so you can get your warnings in restfull manner.

## Compilile and run
```
$ cmake3 .
$ make
# make install
# systemctl start stored
$ wget localhost:1531
```

## Build DEB and RPM
```
$ make
$ make package
$ ls ./build
```

## Uninstall
```
# make uninstall
```

## Tests
```
$ make
$ make tests
```

## Manual Tests
```
$ make valgrind_concurrency
$ make valgrind_memory
# make stress
$ make flawfind
```

## Todo
- funkd compilation without systemd devlibs
- AFL fuzz
- socket activation systemd unit