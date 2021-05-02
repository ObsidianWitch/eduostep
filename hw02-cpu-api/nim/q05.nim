#!/usr/bin/env -S nim r --gc:arc

import os, posix, strformat
import common

let cpid = fork()
if cpid < 0: # error
    raiseOsError osLastError()
elif cpid == 0: # child
    if wait(nil) < 0: echo osErrorMsg osLastError()
    echo &"child {getpid()}"
else: # parent
    raiseOsErrorIf wait(nil) < 0
    echo &"parent {getpid()}"
