#!/bin/bash

set -o errexit -o nounset

time {
    for addr in '7,30,8' '10,11,12,13'; do
    for seekspeed in $(seq 0.1 0.1 5.0); do
        # note: some results are discarded because seek speed must divide
        # evenly into track width
        ./disk.py -a "$addr" -S "$seekspeed" -c \
            | awk '/addr /{a=$3} /seekSpeed/{input=$3} \
                   /TOTALS/{print a, "SeekSpeed:"input, $0}'
    done; done | ./plot.py > plot_q2.png
}

time {
    for addr in '7,30,8' '10,11,12,13'; do
    for rotspeed in $(seq 0.1 0.1 5.0); do
        ./disk.py -a "$addr" -R "$rotspeed" -c \
            | awk '/addr /{a=$3} /rotateSpeed/{input=$3} \
                   /TOTALS/{print a, "RotationSpeed:"input, $0}'
    done; done | ./plot.py > plot_q3.png
}
