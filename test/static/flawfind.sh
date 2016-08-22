#!/usr/bin/env bash

exit 77 #skipped

SRC_DIR=src

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
res=$(${SCRIPT_DIR}/../../vendor/flawfinder -QDnFm 0  ${SCRIPT_DIR}/../../${SRC_DIR})

if [[ ! -z ${res} ]];
then
    echo ${res}
    exit 1
else
    exit 0
fi