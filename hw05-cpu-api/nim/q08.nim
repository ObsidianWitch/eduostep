#!/usr/bin/env -S nim r --gc:arc

import os, posix, strformat
import common

var pipefd: array[2, cint]
raiseOsErrorIf pipe(pipefd) < 0

# child: pipefd[0] -> reader -> stdout
let cpid_reader = fork()
if cpid_reader < 0: # error
    raiseOsError osLastError()
elif cpid_reader == 0: # child
    # close unused write end of pipe
    raiseOsErrorIf close(pipefd[1]) < 0
    # replace stdin w/ pipefd[0]
    raiseOsErrorIf dup2(pipefd[0], STDIN_FILENO) != STDIN_FILENO
    # read from /proc/self/fd/0 (pipe)
    var buffer = newString(64)
    raiseOsErrorIf read(STDIN_FILENO, buffer.cstring, buffer.len) < 0
    echo &"child_reader: {buffer}"
    quit QuitSuccess

# child: stdin -> writer -> pipefd[1]
let cpid_writer = fork()
if cpid_writer < 0: # error
    raiseOsError osLastError()
elif cpid_writer == 0: # child
    # close unused read end of pipe
    raiseOsErrorIf close(pipefd[0]) < 0
    # replace stdout w/ pipefd[1]
    raiseOsErrorIf dup2(pipefd[1], STDOUT_FILENO) != STDOUT_FILENO
    # print to /proc/self/fd/1 (pipe)
    echo "child_writer: Hello, World!"
    quit QuitSuccess

# parent
raiseOsErrorIf close(pipefd[0]) < 0
raiseOsErrorIf close(pipefd[1]) < 0
var wstatus: cint
raiseOsErrorIf waitpid(cpid_writer, wstatus, 0) < 0
raiseOsErrorIf waitpid(cpid_reader, wstatus, 0) < 0
