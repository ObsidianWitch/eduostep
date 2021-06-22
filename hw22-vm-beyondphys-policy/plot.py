#!/usr/bin/env python3

import sys
from matplotlib import pyplot, ticker

# retrieve data
# line example: FIFO 1 FINALSTATS hits 982 misses 99018 hitrate 0.98
data = {}
for line in sys.stdin:
    line = line.split()
    if line[0] not in data:
        data[line[0]] = {}
    data[line[0]][int(line[1])] = float(line[8])
for policy, results in data.items():
    data[policy] = dict(sorted(results.items()))

# plot data
figure, axis = pyplot.subplots()
for i, (policy, results) in enumerate(data.items()):
    axis.plot(results.keys(), results.values(), marker=f'.', \
        linestyle=(0, (1, i)), label=policy)
axis.legend()
axis.set_xlabel("Cache Size (Pages)")
axis.set_ylabel("Hit Rate")
axis.grid()

# show/save plot
if sys.stdout.isatty():
    pyplot.show()
else:
    pyplot.savefig(sys.stdout.buffer, dpi=300)
