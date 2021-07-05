#!/usr/bin/env python3

import sys, statistics
from matplotlib import pyplot

# retrieve data
data = {}
for line in sys.stdin:
    line = dict(kwarg.split('=') for kwarg in line.split())
    program = f'{line["program"].split("/")[-1]} {line["op"]}'
    nthreads = int(line['nthreads'])
    time = float(line['time'])

    if program not in data:
        data[program] = {}
    if nthreads not in data[program]:
        data[program][nthreads] = []
    data[program][nthreads].append(time)

# sort by nthreads & average times
for program, threads in data.items():
    data[program] = dict(sorted(threads.items()))
    data[program] = dict(
        (thread, statistics.mean(times))
        for thread, times in threads.items()
    )

# plot data
figure, axis = pyplot.subplots()
for i, (program, results) in enumerate(data.items()):
    axis.plot(results.keys(), results.values(), marker='.', \
        linestyle=(0, (1, i)), label=program)
axis.legend()
axis.set_xticks(tuple(tuple(data.values())[0].keys()))
axis.set_xlabel("Threads")
axis.set_ylabel("Time (s)")
axis.grid()

# show/save plot
if sys.stdout.isatty():
    pyplot.show()
else:
    pyplot.savefig(sys.stdout.buffer, dpi=300)
