Q1. First let’s make sure you understand how the programs generally work, and some of the key options. Study the code in `vector-deadlock.c`, as well as in `main-common.c` and related files. Now, run `./vector-deadlock -n 2 -l 1 -v`, which instantiates two threads (`-n 2`), each of which does one vector add (`-l 1`), and does so in verbose mode (`-v`). Make sure you understand the output. How does the output change from run to run?

Most of the times the first thread executes first and then the second. The opposite happens rarely.

Q2. Now add the `-d` flag, and change the number of loops (`-l`) from 1 to higher numbers. What happens? Does the code (always) deadlock?

The probability of the code deadlocking increases when the number of loops and/or the number of threads increases.

Q3. How does changing the number of threads (`-n`) change the outcome of the program? Are there any values of `-n` that ensure no deadlock occurs?

No deadlock occurs for `-n1`.

Q4. Now examine the code in `vector-global-order.c`. First, make sure you understand what the code is trying to do; do you understand why the code avoids deadlock? Also, why is there a special case in this `vector_add()` routine when the source and destination vectors are the same?

* This program always locks the vector with the lower address first to avoid deadlocks (total ordering).
* Relocking a default type mutex (`PTHREAD_MUTEX_DEFAULT`) is undefined behaviour (`man 3p pthread_mutex_lock`), the special case is there to avoid this scenario.

Q5. Now run the code with the following flags: `-t -n 2 -l 100000 -d`. How long does the code take to complete? How does the total time change when you increase the number of loops, or the number of threads?

```sh
for _ in {0..99}; do
    out/vector-global-order -n2 -l100000 -dt
done | awk '{ sum += $2 } END { print sum / NR }'
# execution time (averaged over 100 executions): 0.0751 seconds
```

```sh
for nthreads in {1..4}; do
    for nloops in {4..6}; do
        nloops="$((10**nloops))"
        echo -n "$nthreads $nloops "
        for _ in {0..9}; do
            out/vector-global-order -n"$nthreads" -l"$nloops" -dt
        done | awk '{ sum += $2 } END { print sum / NR }'
    done
done
# nthreads nloops  time_avg
# 1        10000   0
# 1        100000  0.011
# 1        1000000 0.133
# 2        10000   0.01
# 2        100000  0.076
# 2        1000000 0.741
# 3        10000   0.019
# 3        100000  0.167
# 3        1000000 1.611
# 4        10000   0.038
# 4        100000  0.36
# 4        1000000 4.081
```

Q6. What happens if you turn on the parallelism flag (`-p`)? How much would you expect performance to change when each thread is working on adding different vectors (which is what `-p` enables) versus working on the same ones?

If threads work on different vectors then there is no need for any locking mechanism. In our cases the locks will still be held but there won't be any contention, thus I expect performances to be better.

```sh
# nthreads nloops  time_avg
# 1        10000   0
# 1        100000  0.01
# 1        1000000 0.13
# 2        10000   0
# 2        100000  0.021
# 2        1000000 0.191
# 3        10000   0
# 3        100000  0.03
# 3        1000000 0.279
# 4        10000   0
# 4        100000  0.031
# 4        1000000 0.341
```

Q7. Now let’s study `vector-try-wait.c`. First make sure you understand the code. Is the first call to `pthread_mutex_trylock()` really needed? Now run the code. How fast does it run compared to the global order approach? How does the number of retries, as counted by the code, change as the number of threads increases?

The first call should be able to be replaced with `pthread_mutex_lock()` without any risks of deadlocks.

```sh
for nthreads in {1..4}; do
    for nloops in {4..6}; do
        nloops="$((10**nloops))"
        echo -n "$nthreads $nloops "
        for _ in {0..9}; do
            out/vector-try-wait -n"$nthreads" -l"$nloops" -dt | tr '\n' ' '
            echo
        done | awk '{ retries += $2; time += $4 } END { print retries / NR, time / NR }'
    done
done
# nthreads nloops  retries_avg time_avg
# 1        10000   0           0
# 1        100000  0           0.01
# 1        1000000 0           0.121
# 2        10000   75421       0.021
# 2        100000  787384      0.176
# 2        1000000 7061443     2.138
# 3        10000   97045       0.031
# 3        100000  1.13004e+06 0.342
# 3        1000000 1.18593e+07 3.442
# 4        10000   226942      0.064
# 4        100000  2.30071e+06 0.667
# 4        1000000 2.37397e+07 6.625
```

Q8. Now let’s look at `vector-avoid-hold-and-wait.c`. What is the main problem with this approach? How does its performance compare to the other versions, when running both with `-p` and without it?

The main problem is that there will be contention to acquire the global lock on each call to `vector_add()`, even when not required (e.g. `-p` flag).

```sh
# without -p
# nthreads nloops  time_avg
# 1        10000   0.002
# 1        100000  0.02
# 1        1000000 0.193
# 2        10000   0.035
# 2        100000  0.411
# 2        1000000 3.429
# 3        10000   0.049
# 3        100000  0.444
# 3        1000000 3.944
# 4        10000   0.033
# 4        100000  0.461
# 4        1000000 4.278

# with -p
# nthreads nloops  time_avg
# 1        10000   0.002
# 1        100000  0.02
# 1        1000000 0.196
# 2        10000   0.008
# 2        100000  0.046
# 2        1000000 0.483
# 3        10000   0.01
# 3        100000  0.087
# 3        1000000 0.841
# 4        10000   0.01
# 4        100000  0.128
# 4        1000000 1.279
```

Q9. Finally, let’s look at `vector-nolock.c`. This version doesn’t use locks at all; does it provide the exact same semantics as the other versions? Why or why not?

It implements (x86 assembly) a `fetch_and_add()` atomic function using the `add` instruction (fetch-and-add) prefixed with `lock` (atomicity).

Q10. Now compare its performance to the other versions, both when threads are working on the same two vectors (no `-p`) and when each thread is working on separate vectors (`-p`). How does this no-lock version perform?

```sh
# without -p
# nthreads nloops  time_avg
# 1        10000   0.011
# 1        100000  0.08
# 1        1000000 0.8
# 2        10000   0.047
# 2        100000  0.422
# 2        1000000 4.889
# 3        10000   0.046
# 3        100000  0.487
# 3        1000000 4.833
# 4        10000   0.059
# 4        100000  0.634
# 4        1000000 6.382

# with -p
# nthreads nloops  time_avg
# 1        10000   0.011
# 1        100000  0.08
# 1        1000000 0.801
# 2        10000   0.011
# 2        100000  0.09
# 2        1000000 0.837
# 3        10000   0.02
# 3        100000  0.181
# 3        1000000 1.548
# 4        10000   0.02
# 4        100000  0.184
# 4        1000000 1.839
```
