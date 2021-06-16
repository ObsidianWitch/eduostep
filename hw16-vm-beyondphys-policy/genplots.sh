#!/bin/bash

set -o errexit -o nounset

generate_policy() {
    local args=()
    if [[ "$2" == CLOCK? ]]; then
        args+=("--policy=${2::5}")
        args+=("--clockbits=${2:5}")
    else
        args+=("--policy=$2")
    fi
    args+=("--cachesize=$3")
    echo -n "$2 $3 "
    ./paging-policy.py --addressfile="$1" "${args[@]}" --compute \
        | tail -n2 | head -n1
}
export -f generate_policy

generate_plot() {
    time ./gentrace.py --workload="$1" > "out/w$1-trace.txt"
    time parallel "generate_policy out/w$1-trace.txt {}" \
        ::: FIFO RAND CLOCK1 CLOCK2 CLOCK3 CLOCK4 LRU OPT \
        ::: 1 {10..100..10} \
        > "out/w$1-results.txt"
    ./plot.py < "out/w$1-results.txt" > "w$1.png"
}

valgrind_plot() {
    local workload='valgrindls'
    time valgrind --tool=lackey --trace-mem=yes ls 2>&1 1>/dev/null \
        | ./addr2vpn.py > "out/w$workload-trace.txt"
    time parallel "generate_policy out/w$workload-trace.txt {}" \
        ::: FIFO RAND CLOCK2 LRU OPT \
        ::: {1..10} {20..50..10} \
        > "out/w$workload-results.txt"
    ./plot.py < "out/w$workload-results.txt" > "w$workload.png"
}

mkdir -p 'out/'
# generate_plot nolocality
# generate_plot step8020
valgrind_plot
