#!/usr/bin/env -S nim r --gc:arc

import os, posix, strformat
import common

proc sigusr1_handler(signum:cint){.noconv.} = discard

var action = Sigaction(sa_handler:sigusr1_handler)
raiseOsErrorIf sigaction(SIGUSR1, action, nil) < 0

let cpid = fork()
if cpid < 0: # error
    raiseOsError osLastError()
elif cpid == 0: # child
    echo &"child {getpid()}: hello"
    raiseOsErrorIf kill(getppid(), SIGUSR1) < 0
else: # parent
    discard pause() # sleep until a **handled** signal is delivered
    echo &"parent {getpid()}: goodbye"
