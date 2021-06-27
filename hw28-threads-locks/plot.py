#!/usr/bin/env python3

import sys
from matplotlib import pyplot

# retrieve data
data = {}
for line in sys.stdin:
    line = line.split()
    if line[0] not in data:
        data[line[0]] = {}
    data[line[0]][int(line[1])] = int(line[2])
for program, results in data.items():
    data[program] = dict(sorted(results.items()))

# plot data
figure, axis = pyplot.subplots()
for i, (program, results) in enumerate(data.items()):
    axis.plot(results.keys(), results.values(), marker=f'.', \
        linestyle=(0, (1, i)), label=program)
axis.legend()
axis.set_xlabel("Threads")
axis.set_ylabel("Instructions")
axis.grid()

# show/save plot
if sys.stdout.isatty():
    pyplot.show()
else:
    pyplot.savefig(sys.stdout.buffer, dpi=300)
