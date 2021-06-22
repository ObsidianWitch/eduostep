#! /usr/bin/env python3

from itertools import accumulate

# store original index before doing any operation on jobs
def Jobs(*values):
    return tuple((i,v) for i,v in enumerate(values))

# FIFO response time: accumulate lengths.
# Tr[0] = 0
# Tr[j] = Tr[j-1] + jobs[j-1].length
def FIFO_Tr(jobs):
    # response time by accumulating lengths
    tr = tuple(accumulate(jobs, lambda accu, v: accu + v[1], initial=0))[:-1]
    # assign response time to result
    return tuple((i, tr[j]) for j, (i, _) in enumerate(jobs))

# SJF response time: sort jobs by ascending length and compute FIFO response
# time.
def SJF_Tr(jobs):
    result = sorted(jobs, key=lambda v: v[1])
    return FIFO_Tr(result)

# RR response time
# Tr[0] = 0
# Tr[j] = Tr[j-1] + min(q, jobs[j-1])
def RR_Tr(q, jobs):
    # response time by accumulating lengths
    tr = tuple(
        accumulate(jobs, lambda accu, v: accu + min(q, v[1]), initial=0)
    )[:-1]
    # assign response time to result
    return tuple((i, tr[j]) for j, (i, _) in enumerate(jobs))

for jobs in (Jobs(1,3,7), Jobs(7,3,1), Jobs(7,7,1)):
    print(jobs, 'jobs')
    print(FIFO_Tr(jobs), 'FIFO_Tr')
    print(SJF_Tr(jobs), 'SJF_Tr')
    print(SJF_Tr(jobs), 'SJF_Tr')
    print(RR_Tr(1, jobs), 'RR_Tr q=1')
    print(RR_Tr(7, jobs), 'RR_Tr q=7')
    print()
