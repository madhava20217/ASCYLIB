#! /bin/bash

#compilation instructions
make clean #clean
make -no-pie STATS=1 TEST=correct
