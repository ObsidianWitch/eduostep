#!/bin/bash

set -o errexit -o nounset

for n in {1000..10000..1000}; do
    out/mem "$n" \
        | head -n 12 \
        | awk '/allocating/ { printf "%s ", substr($4, 2, length($4) - 4)};
               /loop/ { cnt += $7;  printf "%s ", $7 };
               END { print "AVG", cnt / (NR - 2)}'
done
