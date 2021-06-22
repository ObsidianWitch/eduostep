#!/usr/bin/env -S nim r --gc:arc

import os, posix

let cpid = fork()
if cpid < 0: # error
    raiseOsError osLastError()
elif cpid == 0: # child
    discard execlp("ls", "ls", "-l", nil)
    raiseOsError osLastError()
