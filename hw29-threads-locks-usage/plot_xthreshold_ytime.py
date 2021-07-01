#!/usr/bin/env python3

import sys
from matplotlib import pyplot

# retrieve data
data = {}
for line in sys.stdin:
    line = line.split()
    threshold = int(line[0])
    result = float(line[2])
    data[threshold] = result
data = dict(sorted(data.items()))

# plot data
figure, axis = pyplot.subplots()
axis.plot(data.keys(), data.values(), marker='.')
axis.set_xscale('log', base=2)
axis.set_xticks(tuple(data.keys()))
axis.set_xlabel("Threshold")
axis.set_ylabel("Time (s)")
axis.grid()

# show/save plot
if sys.stdout.isatty():
    pyplot.show()
else:
    pyplot.savefig(sys.stdout.buffer, dpi=300)
