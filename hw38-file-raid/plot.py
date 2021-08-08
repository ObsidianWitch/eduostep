#!/usr/bin/env python3

import sys, statistics
from matplotlib import pyplot

# retrieve data
data = {}
for line in sys.stdin:
    line = line.split()
    level = f"level {line[0]}"
    ndisks = int(line[1])
    time = float(line[4])

    if level not in data:
        data[level] = {}
    data[level][ndisks] = time

# plot data
figure, axis = pyplot.subplots()
for i, (level, results) in enumerate(data.items()):
    axis.plot(results.keys(), results.values(), marker='.', \
        linestyle=(0, (1, i)), label=level)
axis.legend()
axis.set_xticks(tuple(tuple(data.values())[0].keys()))
axis.set_xlabel("Disks")
axis.set_ylabel("Time")
axis.grid()

# show/save plot
if sys.stdout.isatty():
    pyplot.show()
else:
    pyplot.savefig(sys.stdout.buffer, dpi=300)
