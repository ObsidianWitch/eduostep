#!/bin/bash

set -o errexit -o nounset

cargo build --release
trap 'pkill --parent "$$"' EXIT
for bin in 'http_sync' 'http_async'; do
    for page in '/' '/sleep'; do
        "target/release/$bin" > /dev/null &
        sleep 1 # wait for server to start
        echo -n "$bin $page"
        time parallel -n0 "curl --silent 127.0.0.1:7878$page > /dev/null" \
            ::: {1..1000}
        echo
        pkill "$bin"
    done
done

# Benchmark synchronous and asynchronous http server with 1000 requests sent in
# parallel (1 request/CPU). Two types of requests are tested: requests without
# any sleep and requests with 100ms of sleep (simulate latency). Both servers
# perform similarly on fast requests, but the async server perform better than
# the sync server when requests are slower.
#
# http_sync /
# real 0m5.410s
# user 0m12.328s
# sys  0m5.689s
#
# http_sync /sleep
# real 1m40.602s
# user 0m22.035s
# sys  0m13.125s
#
# http_async /
# real 0m5.521s
# user 0m12.357s
# sys  0m5.677s
#
# http_async /sleep
# real 0m33.865s
# user 0m22.151s
# sys  0m13.516s
