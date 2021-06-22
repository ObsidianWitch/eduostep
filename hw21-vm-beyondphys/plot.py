#!/usr/bin/env python3

import sys
from matplotlib import pyplot, ticker

# retrieve data
data = {}
for line in sys.stdin:
    line = line.split()
    data[int(line[0])] = float(line[-1])

# plot data
figure, axis = pyplot.subplots()
axis.plot(data.keys(), data.values(), marker="o")
axis.set_ylim(ymin=0)
axis.set_xlabel("size (MiB)")
axis.set_ylabel("average bandwidth (MiB/s)")
axis.grid()

# show/save plot
if sys.stdout.isatty():
    pyplot.show()
else:
    pyplot.savefig(sys.stdout.buffer)
