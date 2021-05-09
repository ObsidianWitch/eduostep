1. The first Linux tool you should check out is the very simple tool free. First, type man free and read its entire manual page; it’s short, don’t worry!

2. Now, run free, perhaps using some of the arguments that might be useful (e.g., -m, to display memory totals in ~~megabytes~~ mebibytes). How much memory is in your system? How much is free? Do these numbers match your intuition?

    ```sh
    $ free --mebi
            total        used        free      shared  buff/cache   available
    Mem:    11859        6645        2796        1286        2417        3494
    Swap:   12287        2798        9489
    ```

    There are 2'796 MiB of **free** (unused) memory, which is lower than the 3'494 MiB of **available** memory.

    ```
    $ man 1 free
    total  Total installed memory (MemTotal and SwapTotal in /proc/meminfo)

    used   Used memory (calculated as total - free - buffers - cache)

    free   Unused memory (MemFree and SwapFree in /proc/meminfo)

    shared Memory used (mostly) by tmpfs (Shmem in /proc/meminfo)

    buffers
           Memory used by kernel buffers (Buffers in /proc/meminfo)

    cache  Memory used by the page cache and slabs (Cached and SReclaimable in
           /proc/meminfo)

    buff/cache
           Sum of buffers and cache

    available
           Estimation  of how much memory is available for starting new appli‐
           cations, without swapping. Unlike the data provided by the cache or
           free fields, this field takes into account page cache and also that
           not all reclaimable memory slabs will be reclaimed due to items be‐
           ing  in  use  (MemAvailable  in /proc/meminfo, available on kernels
           3.14, emulated on kernels 2.6.27+, otherwise the same as free)
    ```

3. Next, create a little program that uses a certain amount of memory, called memory-user.c. This program should take one command-line argument: the number of megabytes of memory it will use. When run, it should allocate an array, and constantly stream through the array, touching each entry. The program should do this indefinitely, or, perhaps, for a certain amount of time also specified at the command line.

    See `memory-user.c`.

4. Now, while running your memory-user program, also (in a different terminal window, but on the same machine) run the free tool. How do the memory usage totals change when your program is running? How about when you kill the memory-user program? Do the numbers match your expectations? Try this for different amounts of memory usage. What happens when you use really large amounts of memory?

    ```sh
    $ free -m && out/memory-user 1000 10 & \
      sleep 1 && free -m && wait && free -m
    [1] 2074430
            total        used        free      shared  buff/cache   available
    Mem:    11859        5874        3909        1314        2075        4326
    Swap:   12287        3854        8433
            total        used        free      shared  buff/cache   available
    Mem:    11859        6828        2965        1304        2064        3383
    Swap:   12287        3854        8433
    [1]+  Done                free -m && out/memory-user 1000 10
            total        used        free      shared  buff/cache   available
    Mem:    11859        5863        3943        1291        2052        4361
    Swap:   12287        3854        8433
    ```

    ```sh
    $ free -m && out/memory-user 1000 10 & \
      sleep 1 && free -m && pkill -9 memory-user && wait && free -m
    [1] 2093519
          total        used        free      shared  buff/cache   available
    Mem:    11859        6949        3419         834        1490        3748
    Swap:   12287        4617        7670
          total        used        free      shared  buff/cache   available
    Mem:    11859        7902        2466         834        1490        2795
    Swap:   12287        4617        7670
    /bin/bash: line 34: 2093522 Killed                  out/memory-user 1000 10
    [1]+  Exit 137                free -m && out/memory-user 1000 10
          total        used        free      shared  buff/cache   available
    Mem:    11859        6948        3426         828        1484        3756
    Swap:   12287        4617        7670
    ```

    ```sh
    $ out/memory-user 100000 0
    calloc: Cannot allocate memory
    ```

5. Let’s try one more tool, known as pmap. Spend some time, and read the pmap manual page in detail.

6. To use pmap, you have to know the process ID of the process you’re interested in. Thus, first run ps auxw to see a list of all processes; then, pick an interesting one, such as a browser. You can also use your memory-user program in this case (indeed, you can even have that program call getpid() and print out its PID for your convenience).

    ```sh
    $ pmap -x "$(pgrep firefox)" | less
    ...
    ```

7. Now run pmap on some of these processes, using various flags (like -X) to reveal many details about the process. What do you see? How many different entities make up a modern address space, as opposed to our simple conception of code/stack/heap?

    * code
    * heap
    * memory-mapped files
    * shared libraries (e.g. `libc-2.33.so`, `ld-2.33.so`)
    * stack
    * vDSO: small shared library mapped into the address space of all user-space applications by the kernel. Accelerate the execution of specific system calls that do not necessarily have to run in kernel space (avoid context switch).  (man 7 VDSO, https://lwn.net/Articles/615809/)
    * vvar: memory region used by vDSO to retrieve variables from the kernel address space (read-only) (https://lwn.net/Articles/615809/)
    * vsyscall: deprecated, less flexible than vDSO (https://lwn.net/Articles/604515/)

8. Finally, let’s run pmap on your memory-user program, with different amounts of used memory. What do you see here? Does the output from pmap match your expectations?

    ```sh
    $ out/memory-user 1000 5 & sleep 1 && pmap -x "$!"
    [1] 2169165
    2169165:   out/memory-user 1000 5
    Address           Kbytes     RSS   Dirty Mode  Mapping
    000055f63a483000       4       4       0 r---- memory-user
    000055f63a484000       4       4       0 r-x-- memory-user
    000055f63a485000       4       0       0 r---- memory-user
    000055f63a486000       4       4       4 r---- memory-user
    000055f63a487000       4       4       4 rw--- memory-user
    000055f63bb52000     132       4       4 rw---   [ anon ]
    00007fccc394b000  976572  976568  976568 rw---   [ anon ]
    00007fccff2fa000     152     152       0 r---- libc-2.33.so
    00007fccff320000    1328     636       0 r-x-- libc-2.33.so
    00007fccff46c000     304      60       0 r---- libc-2.33.so
    00007fccff4b8000      12      12      12 r---- libc-2.33.so
    00007fccff4bb000      12      12      12 rw--- libc-2.33.so
    00007fccff4be000      44      20      20 rw---   [ anon ]
    00007fccff4e8000       4       4       0 r---- ld-2.33.so
    00007fccff4e9000     144     144       0 r-x-- ld-2.33.so
    00007fccff50d000      36      36       0 r---- ld-2.33.so
    00007fccff517000       8       8       8 r---- ld-2.33.so
    00007fccff519000       8       8       8 rw--- ld-2.33.so
    00007ffd67897000     132      12      12 rw---   [ stack ]
    00007ffd679ec000      16       0       0 r----   [ anon ]
    00007ffd679f0000       8       4       0 r-x--   [ anon ]
    ffffffffff600000       4       0       0 --x--   [ anon ]
    ---------------- ------- ------- -------
    total kB          978936  977696  976652
    ```

    `memory-user` allocates 1'000 MB of memory which can be found at address `00007fccc394b000` with size 976'572 KiB. 1'000 MB = 10^9 B = 10^9 / 2^10 KiB ~= 976'562 KiB, so there's a difference of 10 KiB between what was expected and reality.
