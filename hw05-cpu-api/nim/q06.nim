#!/usr/bin/env -S nim r --gc:arc

import os, posix, strformat
import common

let cpid = fork()
if cpid < 0: # error
    raiseOsError osLastError()
elif cpid == 0: # child
    echo &"child {getpid()}"
else: # parent
    var wstatus:cint
    raiseOsErrorIf waitpid(cpid, wstatus, 0) < 0
    echo &"parent {getpid()}: {wstatus=}"
