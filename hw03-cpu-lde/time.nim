#!/usr/bin/env -S nim r --gc:arc -d:release

import os, posix, strformat
import sched

const debug = not defined(release)

proc raiseOsErrorIf(failure:bool, msg = "") =
    if failure: raiseOsError(osLastError(), msg)

proc getTime(clockId: ClockId = CLOCK_PROCESS_CPUTIME_ID): Timespec =
    raiseOsErrorIf clock_gettime(clockId, result) < 0
    return result

# Return `ta - tb` in nanoseconds.
proc `-`(ta: Timespec, tb: Timespec): int =
    return (ta.tv_nsec - tb.tv_nsec) +
           (ta.tv_sec.clong - tb.tv_sec.clong) * 1_000_000_000

# ---

# Retrieve and print CLOCK_PROCESS_CPUTIME_ID's resolution.
proc clockResolution() =
    var res: Timespec
    raiseOsErrorIf clock_getres(CLOCK_PROCESS_CPUTIME_ID, res) < 0
    echo &"clock resolution: {res.tv_sec.clong}.{res.tv_nsec:09} s"

# Use a simple syscall (0B read) `n` times & return the average execution time
# of this call in nanoseconds.
# NB: "Applications should use the clock_gettime() function instead of the
# obsolescent gettimeofday() function." -man 3p gettimeofday
proc timeSyscall(n: Natural): int =
    let ts1 = getTime()
    for i in 0..<n:
        discard read(STDIN_FILENO, nil, 0)
    let ts2 = getTime()

    result = (ts2 - ts1) div n
    echo &"time syscall: {result} ns"
    return result

# Measure the time it takes to write and read a token `n` times through a pipe
# on one process (no context switch).
proc timePipe(n: Natural): int =
    var pipefd: array[2, cint]
    raiseOsErrorIf pipe(pipefd) < 0

    var buffer = 'p'
    let ts1 = getTime()
    for i in 0..<n:
        discard write(pipefd[1], addr buffer, 1)
        discard read(pipefd[0], addr buffer, 1)
    let ts2 = getTime()

    # close pipes
    raiseOsErrorIf close(pipefd[0]) < 0
    raiseOsErrorIf close(pipefd[1]) < 0

    # result
    result = (ts2 - ts1) div n
    echo &"time pipe: {result} ns"
    return result

# Time context switch between two processes on one processor by using a loop of
# blocking ipc.
proc timeCtxswitch(n: Natural, syscallNsec: int, pipeNsec: int) =
    # prepare pipes for two-way IPC between parent and child
    # parent -1-pipes[0]-0-> child
    # child  -1-pipes[1]-0-> parent
    var pipes: array[2, array[2, cint]]
    raiseOsErrorIf pipe(pipes[0]) < 0
    raiseOsErrorIf pipe(pipes[1]) < 0

    # run current thread on cpu 0
    # "A child created via fork(2) inherits its parent's CPU affinity mask."
    # -man 2 sched_getaffinity
    raiseOsErrorIf setAffinity(pid=0, cpu=0) < 0

    let cpid = fork()
    if cpid < 0: # error
        raiseOsError osLastError()
    elif cpid == 0: # child
        # close unused pipes
        raiseOsErrorIf(close(pipes[0][1]) < 0, "[c] close pipe01")
        raiseOsErrorIf(close(pipes[1][0]) < 0, "[c] close pipe10")

        # blocking ipc loop to trigger context switch
        var whoami = 'c'
        var buffer = ' '
        for i in 0..<n:
            discard read(pipes[0][0], addr buffer, 1)
            when debug: echo &"[c][{i=}] {buffer}"
            discard write(pipes[1][1], addr whoami, 1)

        # close pipes
        raiseOsErrorIf(close(pipes[0][0]) < 0, "[c] close pipe00")
        raiseOsErrorIf(close(pipes[1][1]) < 0, "[c] close pipe11")
    else: # parent
        # close unused pipes
        raiseOsErrorIf(close(pipes[0][0]) < 0, "[p] close pipe00")
        raiseOsErrorIf(close(pipes[1][1]) < 0, "[p] close pipe11")

        # blocking ipc loop to trigger context switch
        var whoami = 'p'
        var buffer = '_'
        let ts1 = getTime()
        for i in 0..<n:
            discard write(pipes[0][1], addr whoami, 1)
            discard read(pipes[1][0], addr buffer, 1)
            when debug: echo &"[p][{i=}] buffer"
        let ts2 = getTime()

        # close pipes
        raiseOsErrorIf(close(pipes[0][1]) < 0, "[p] close pipe01")
        raiseOsErrorIf(close(pipes[1][0]) < 0, "[p] close pipe10")

        # results
        # estimate duration of one iteration of the ipc loop from the parent
        let ipcNsec = (ts2 - ts1) div n
        echo &"time ctxsw (w/ overhead): {ipcNsec} ns"

        # estimate duration of context switch w/o 2 syscalls overhead
        let ctxsw1Nsec = ipcNsec - (2 * syscallNsec)
        echo &"time ctxsw (w/o 2*syscall overhead): {ctxsw1Nsec} ns"

        # estimate duration of context switch w/o pipe overhead
        let ctxsw2Nsec = ipcNsec - pipeNsec
        echo &"time ctxsw (w/o pipe overhead): {ctxsw2Nsec} ns"

# Objectives: measure the costs of a system call and context switch.
clockResolution()
let syscallNsec = timeSyscall(1000000)
let pipeNsec = timePipe(1000000)
timeCtxswitch(1000000, syscallNsec, pipeNsec)
