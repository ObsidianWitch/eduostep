#!/usr/bin/env python3

import argparse
from functools import reduce

class Job:
    STRIDE_DIVIDEND = 10000

    def __init__(self, length, tickets):
        self.length = length
        self.tickets = tickets
        self.stride = self.STRIDE_DIVIDEND // length
        self.pass_ = 0
        self.runtime = 0

def schedule(quantum, jobs):
    print("** Solutions **")
    tick = 0
    while jobs:
        # select job with minimum pass, O(n)
        current = reduce(lambda x, y: y if y.pass_ < x.pass_ else x, jobs)

        # schedule current job
        print(tick, current.__dict__)
        current.runtime += min(quantum, current.length - current.runtime)
        current.pass_ += current.stride
        print(tick, current.__dict__)

        if current.runtime >= current.length:
            jobs.remove(current) # O(n)
            print(f"--> JOB _ DONE at time {tick}")

        tick += 1

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('jlist', nargs='+', help='length:tickets')
    parser.add_argument('--quantum', default=1)
    args = parser.parse_args()

    jobs = [ Job(length=int(split[0]), tickets=int(split[1]))
             for job in args.jlist
             if (split := job.split(':')) ]
    schedule(args.quantum, jobs)
