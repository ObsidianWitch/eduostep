#!/usr/bin/env -S nim r --gc:arc

import os, posix, strformat
import common

let cpid = fork()
if cpid < 0: # error
    raiseOsError osLastError()
elif cpid == 0: # child
    raiseOsErrorIf close(STDOUT_FILENO) < 0
    echo &"child {getpid()}"
else: # parent
    echo &"parent {getpid()}"
