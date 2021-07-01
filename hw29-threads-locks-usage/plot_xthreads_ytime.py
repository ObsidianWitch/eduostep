#!/usr/bin/env python3

import sys
from matplotlib import pyplot

# retrieve data
data = {}
for line in sys.stdin:
    line = line.split()
    program = line[0]
    nthreads = int(line[1])
    result = float(line[3])
    if program not in data:
        data[program] = {}
    data[program][nthreads] = result
for program, results in data.items():
    data[program] = dict(sorted(results.items()))

# plot data
figure, axis = pyplot.subplots()
for i, (program, results) in enumerate(data.items()):
    axis.plot(results.keys(), results.values(), marker='.', \
        linestyle=(0, (1, i)), label=program)
axis.legend()
axis.set_xlabel("Threads")
axis.set_ylabel("Time (s)")
axis.grid()

# show/save plot
if sys.stdout.isatty():
    pyplot.show()
else:
    pyplot.savefig(sys.stdout.buffer, dpi=300)
