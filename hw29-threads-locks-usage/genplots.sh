#!/bin/bash

set -o errexit -o nounset

time {
    for program in simple_counter sloppy_counter; do
    for thread in {1..4}; do
    for experiment in {1..100}; do
        echo -n "$program $thread "
        "out/$program" "$thread" 1000000
    done; done; done | ./plot_xthreads_ytime.py > counters_plot.png
}

time {
    for i in {0..10}; do
        threshold="$((2**i))"
        for experiment in {1..100}; do
            echo -n "$threshold ";
            out/sloppy_counter 4 1000000 "$threshold"
        done
    done | ./plot_xthreshold_ytime.py > threshold_plot.png
}
