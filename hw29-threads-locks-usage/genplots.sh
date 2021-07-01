#!/bin/bash

set -o errexit -o nounset -o xtrace

time parallel 'out/simple_counter {} 100000000' ::: {1..10} \
    | ./plot.py > q2plot.png
