#!/usr/bin/env -S nim r --gc:arc

# OwnQ09. Implement a pipe of two commands using pipe().
# ps | grep --color --extended-regexp 'ps|grep|$'

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
    # execute rightmost command
    discard execlp("grep", "grep", "--color", "--extended-regexp",
                   "ps|grep|$", nil)
    raiseOsError osLastError()


# child: stdin -> writer -> pipefd[1]
let cpid_writer = fork()
if cpid_writer < 0: # error
    raiseOsError osLastError()
elif cpid_writer == 0: # child
    # close unused read end of pipe
    raiseOsErrorIf close(pipefd[0]) < 0
    # replace stdout w/ pipefd[1]
    raiseOsErrorIf dup2(pipefd[1], STDOUT_FILENO) != STDOUT_FILENO
    # execute leftmost command
    discard execlp("ps", "ps", nil)
    raiseOsError osLastError()

# parent
raiseOsErrorIf close(pipefd[0]) < 0
raiseOsErrorIf close(pipefd[1]) < 0
# simulate shell waiting for its current command to finish executing
# before executing the next one
var wstatus: cint
raiseOsErrorIf waitpid(cpid_writer, wstatus, 0) < 0
raiseOsErrorIf waitpid(cpid_reader, wstatus, 0) < 0
