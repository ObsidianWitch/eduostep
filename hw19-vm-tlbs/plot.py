#!/usr/bin/env python3

import sys
from matplotlib import pyplot, ticker

# retrieve data
data = {}
for line in sys.stdin:
    if not line.startswith("CST"): continue
    line = line.split()
    data[int(line[5])] = float(line[8])

# plot data
figure, axis = pyplot.subplots()
axis.plot(data.keys(), data.values(), marker="o")
axis.set_xscale("log", base=2)
axis.xaxis.set_major_locator(ticker.LogLocator(base=2,
    numticks=len(data.keys()) + 1))
axis.set_xlabel("Number of pages")
axis.set_ylabel("Time per access (ns)")
axis.set_title("TLB size measurement")
axis.grid()

# show/save plot
if sys.stdout.isatty():
    pyplot.show()
else:
    pyplot.savefig(sys.stdout.buffer)
