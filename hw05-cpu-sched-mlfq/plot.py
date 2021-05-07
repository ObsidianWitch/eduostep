#!/usr/bin/env python3

import sys, re
from matplotlib import pyplot, colors # dep: https://pypi.org/project/matplotlib/

# retrieve data
queues = {} # each queue contains a dict of jobs, and each job contains a list
            # of times
reTrace = re.compile("^\[ time ([0-9]+) \] Run JOB ([0-9]+) at PRIORITY ([0-9]+)")
for line in sys.stdin:
    if match := reTrace.match(line):
        groups = match.groups()
        groups = tuple(int(item) for item in groups)
        if groups[2] not in queues:
            queues[groups[2]] = {}
        if groups[1] not in queues[groups[2]]:
            queues[groups[2]][groups[1]] = []
        queues[groups[2]][groups[1]].append(groups[0])

# plot data
# ref: https://matplotlib.org/stable/gallery/lines_bars_and_markers/broken_barh.html
figure, axis = pyplot.subplots()
axis.set_yticks(list(queues.keys()))
axis.set_xlabel("time (ms)")
axis.set_ylabel("priority")
for priority, queue in queues.items():
    for ijob, job in queue.items():
        axis.broken_barh(xranges   = tuple((time, 1) for time in job),
                         yrange    = (priority, 1),
                         facecolor = list(colors.TABLEAU_COLORS.values())[ijob])

if sys.stdout.isatty():
    pyplot.show()
else:
    pyplot.savefig(sys.stdout.buffer)
