#!/usr/bin/env python3

import sys, json
from matplotlib import pyplot, ticker

# retrieve data
data = {}
with open(sys.argv[1]) as file:
    for line in file:
        json_data = json.loads(line)
        if json_data["reason"] != "benchmark-complete": continue
        name = json_data["id"].split()
        name, exp = name[0], int(name[1].removeprefix("target/rand_"))
        assert(json_data["typical"]["unit"] == "ns")
        time = float(json_data["typical"]["estimate"]) / 10**6

        if name not in data:
            data[name] = {}
        data[name][exp] = time

# plot data
figure, axis = pyplot.subplots()
for i, (name, results) in enumerate(data.items()):
    axis.plot(results.keys(), results.values(), marker='.', \
        linestyle=(0, (1, i)), label=name)
axis.legend()
axis.set_xlabel("Input File Size (KiB)")
axis.set_ylabel("Time (ms)")
axis.grid()

# show/save plot
if sys.stdout.isatty():
    pyplot.show()
else:
    pyplot.savefig(sys.stdout.buffer, dpi=300)
