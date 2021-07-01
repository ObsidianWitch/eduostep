#!/usr/bin/env python3

import sys
from matplotlib import pyplot, ticker

# retrieve data
data = {}
for line in sys.stdin:
    line = line.split()
    data[int(line[1])] = float(line[5])
data = dict(sorted(data.items()))

# plot data
figure, axis = pyplot.subplots()
axis.plot(data.keys(), data.values(), marker='.')
axis.set_xlabel("Threads")
axis.set_ylabel("Time (s)")
axis.grid()

# show/save plot
if sys.stdout.isatty():
    pyplot.show()
else:
    pyplot.savefig(sys.stdout.buffer, dpi=300)
