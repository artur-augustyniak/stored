STORED
======

## Motivation
For fun.
Autotools and glibc/Linux as development platform, for webdeveloper:D

## Contents
This little project puts syslog msg. when any mount point has less than 30%  space.
It has also tiny builtin web server so you can get your warnings in restfull manner.

## Compilile and run
Classic `./configure && make && make install`. Right now install step is not necessary.
Since configuration still uses `./src/custom_config.h` you can run just build artifact `./src/stored`.
Then:
```
curl -i '127.0.0.1:8081/'
```