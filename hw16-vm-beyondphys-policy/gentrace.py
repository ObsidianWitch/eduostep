#!/usr/bin/env python3

import argparse, random, math
import scipy.stats

parser = argparse.ArgumentParser()
parser.add_argument('--naddrs', type=int, default=100_000)
parser.add_argument('--maxpage', type=int, default=100)
parser.add_argument('--workload', choices=('nolocality', 'step8020',
    'pareto8020'), default='nolocality')
args = parser.parse_args()

if args.workload == 'nolocality':
    weights = None
elif args.workload == 'step8020':
    weights =  [80 / (20 * args.maxpage)] * (20 * args.maxpage // 100)
    weights += [20 / (80 * args.maxpage)] * (args.maxpage - len(weights))
elif args.workload == 'pareto8020':
    weights = scipy.stats.pareto.pdf(range(args.maxpage), math.log(5, 4))
accesses = random.choices(range(args.maxpage), weights=weights, k=args.naddrs)
print('\n'.join(str(d) for d in accesses))
