#!/usr/bin/env python3

import sys, statistics
from matplotlib import pyplot

# retrieve data
data = {}
for line in sys.stdin:
    line = dict(kwarg.split('=') for kwarg in line.split())
    threshold = int(line['threshold'])
    time = float(line['time'])

    if threshold not in data:
        data[threshold] = []
    data[threshold].append(time)

# sort by threshold and average times
data = dict(sorted(data.items()))
data = dict(
    (threshold, statistics.mean(times))
    for threshold, times in data.items()
)

# plot data
figure, axis = pyplot.subplots()
axis.plot(data.keys(), data.values(), marker='.')
axis.set_xscale('log', base=2)
axis.set_xticks(tuple(data.keys()))
axis.set_xlabel("Threshold")
axis.set_ylim(ymin=0)
axis.set_ylabel("Time (s)")
axis.grid()

# show/save plot
if sys.stdout.isatty():
    pyplot.show()
else:
    pyplot.savefig(sys.stdout.buffer, dpi=300)
