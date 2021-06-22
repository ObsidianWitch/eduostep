#!/usr/bin/env -S nim r --gc:arc

import os, posix, strformat

var x = 100
let cpid = fork()
if cpid < 0: # error
    raiseOsError osLastError()
elif cpid == 0: # child
    echo &"child: x={x}" # child: x=100
    x += 1
    echo &"child: x={x}" # child: x=101
else: # parent
    echo &"parent: x={x}" # parent: x=100
    x -= 1
    echo &"parent: x={x}" # parent: x=99
