#!/usr/bin/env python3

import sys, re
from matplotlib import pyplot, colors # dep: https://pypi.org/project/matplotlib/

# retrieve data
data = {}
for line in sys.stdin:
    split = line.split()
    data[int(split[0])] = float(split[1])
data = dict(sorted(data.items()))

# plot data
# ref: https://matplotlib.org/stable/gallery/lines_bars_and_markers/simple_plot.html
figure, axis = pyplot.subplots()
axis.set_xlabel("length")
axis.set_ylabel("unfairness")
axis.plot(data.keys(), data.values())
axis.grid()

if sys.stdout.isatty():
    pyplot.show()
else:
    pyplot.savefig(sys.stdout.buffer)
