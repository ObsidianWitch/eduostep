# ref: https://github.com/nim-lang/Nim/blob/devel/lib/system/threadlocalstorage.nim
import posix

type CpuSet {.importc: "cpu_set_t", header: "<sched.h>".} = object
    when defined(linux) and defined(amd64):
        abi: array[1024 div (8 * sizeof(culong)), culong]

proc cpusetZero(s: var CpuSet) {.importc: "CPU_ZERO", header: "<sched.h>".}

proc cpusetIncl(cpu: cint; s: var CpuSet) {.importc: "CPU_SET",
    header: "<sched.h>".}

proc setAffinity(pid: Pid; setsize: csize_t; s: var CpuSet): cint {.importc:
    "sched_setaffinity", header: "<sched.h>".}

# Set CPU affinity for the current thread.
# ref: https://github.com/nim-lang/Nim/blob/version-1-4/lib/system/threads.nim#L330
proc setAffinity*(pid: Pid, cpu: cint): cint =
    var cpuset {.noinit.}: CpuSet
    cpusetZero(cpuset)
    cpusetIncl(cpu, cpuset)
    return setAffinity(pid, csize_t(sizeof(cpuset)), cpuset)
