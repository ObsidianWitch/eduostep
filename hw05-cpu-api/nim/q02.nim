#!/usr/bin/env -S nim r --gc:arc

import posix
import common

raiseOsErrorIf mkdir("out", 0) < 0 and errno != EEXIST
let fd = open("out/q02.out", O_CREAT or O_TRUNC or O_WRONLY, S_IRUSR or S_IWUSR)
raiseOsErrorIf fd < 0
try:
    let cpid = fork()
    raiseOsErrorIf cpid < 0

    let who:cstring = (if cpid == 0: "parent " else: "child ")
    for _ in 0..100:
        raiseOsErrorIf write(fd, who, who.len) < 0
finally:
    raiseOsErrorIf close(fd) < 0
