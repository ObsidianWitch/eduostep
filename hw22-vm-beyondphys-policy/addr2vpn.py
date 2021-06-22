#!/usr/bin/env python3

import sys

for line in sys.stdin:
    if line.startswith('=='): continue
    address = line[3:].split(',')[0]
    vpn = int(address, 16) >> 12
    print(vpn)
