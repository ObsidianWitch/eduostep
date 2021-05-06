#!/usr/bin/env python3

import sys
from more_itertools import pairwise # https://pypi.org/project/more-itertools/

# Compute average unfairness of all the experiments.
def unfairness_all():
    experiments = []
    for line in sys.stdin:
        if line == "** Solutions **\n":
            experiments.append([])
        elif line.startswith('-->'):
            experiments[-1].append(int(line.split()[6]))

    unfairness = [ unfairness_one(experiment) for experiment in experiments ]
    unfairness = sum(unfairness) / len(unfairness)
    return unfairness

# Compute average unfairness of all the jobs in one experiment.
def unfairness_one(experiment):
    unfairness = [ j1c / j2c for j1c, j2c in pairwise(experiment) ]
    unfairness = sum(unfairness) / len(unfairness)
    return unfairness

print(unfairness_all())
