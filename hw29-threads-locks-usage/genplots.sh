#!/bin/bash

set -o errexit -o nounset

time parallel 'echo -n "{} "; out/{1} {2} 100000000' \
    ::: simple_counter sloppy_counter ::: {1..10} \
    | ./plot_xthreads_ytime.py > counters_plot.png

time parallel 'echo -n "{} "; out/sloppy_counter 4 100000000 {1}' \
    ::: "$(for i in {0..10}; do echo "$((2**i))"; done)" \
    | ./plot_xthreshold_ytime.py > threshold_plot.png
