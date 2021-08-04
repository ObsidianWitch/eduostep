#!/usr/bin/env python3

import sys, statistics
from matplotlib import pyplot

# retrieve data
data = {}
xlabel = "TODO"
for line in sys.stdin:
    line = line.split()
    addr = line[0]
    input = line[1].split(':')
    xlabel, input = input[0], float(input[1])
    times = dict( kwarg.split(':') for kwarg in line[3:] )
    if addr not in data:
        data[addr] = {}
    for k, v in times.items():
        if k not in data[addr]:
            data[addr][k] = {}
        data[addr][k][input] = int(v)

# plot data
figure, axes = pyplot.subplots(nrows=len(data))
for i, (addr, times) in enumerate(data.items()):
    for j, (time_type, results) in enumerate(times.items()):
        axes[i].plot(results.keys(), results.values(), marker='.', \
                  linestyle=(0, (1, j)),
                  label=f'{addr}_{time_type}')
    axes[i].legend()
    axes[-1].set_xlabel(xlabel)
    axes[i].set_ylabel("Time")
    axes[i].grid()

# show/save plot
if sys.stdout.isatty():
    pyplot.show()
else:
    pyplot.savefig(sys.stdout.buffer, dpi=300)
