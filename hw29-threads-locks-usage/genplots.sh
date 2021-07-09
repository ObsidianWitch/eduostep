#!/bin/bash

set -o errexit -o nounset

time {
    for program in counter_simple counter_sloppy; do
    for nthreads in {1..4}; do
    for experiment in {1..100}; do
        "out/$program" "$nthreads" 1000000
    done; done; done | ./plot_xthreads_ytime.py > plot_counters.png
}

time {
    for i in {0..10}; do
        threshold="$((2**i))"
        for experiment in {1..100}; do
            out/counter_sloppy 4 1000000 "$threshold"
        done
    done | ./plot_xthreshold_ytime.py > plot_threshold.png
}

time {
    for program in list_simple list_hoh; do
    for nthreads in {1..4}; do
    for experiment in {1..100}; do
        "out/$program" "$nthreads" 1000000
    done; done; done | ./plot_xthreads_ytime.py > plot_lists.png
}

time {
    for program in bst_simple bst_better; do
    for nthreads in {1..4}; do
    for experiment in {1..100}; do
        "out/$program" "$nthreads" 10000
    done; done; done | ./plot_xthreads_ytime.py > plot_bst.png
}
