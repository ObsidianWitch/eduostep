#!/usr/bin/env python3

import sys
from matplotlib import pyplot, ticker

# retrieve data
data = {}
limit = None
for line in sys.stdin:
    if line.startswith("  Limit  :"):
        limit = int(line.split()[2])
        if limit not in data:
            data[limit] = []
    elif line.startswith("Valid addresses:"):
        data[limit].append(float(line.split()[3]))
data = dict(sorted(data.items()))
mean = tuple(sum(v) / len(v) for v in data.values())

# plot data
figure, axis = pyplot.subplots()
axis.plot(data.keys(), data.values(), "o", color="black")
axis.plot(data.keys(), mean)
axis.set_xlabel("limit")
axis.set_ylabel("valid fraction")
axis.grid()

# show/save plot
if sys.stdout.isatty():
    pyplot.show()
else:
    pyplot.savefig(sys.stdout.buffer)
