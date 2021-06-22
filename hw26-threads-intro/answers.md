**Q1**. Let’s examine a simple program, “loop.s”. First, just read and understand it. Then, run it with these arguments (`./x86.py -p loop.s -t 1 -i 100 -R dx`) This specifies a single thread, an interrupt every 100 instructions, and tracing of register `%dx`. What will `%dx` be during the run? Use the `-c` flag to check your answers; the answers, on the left, show the value of the register (or memory value) after the instruction on the right has run.

```
dx   Thread 0
 0
-1   1000 sub  $1,%dx
-1   1001 test $0,%dx
-1   1002 jgte .top
-1   1003 halt
```

**Q2**. Same code, different flags: (`./x86.py -p loop.s -t 2 -i 100 -a dx=3,dx=3 -R dx`) This specifies two threads, and initializes each `%dx` to 3. What values will `%dx` see? Run with `-c` to check. Does the presence of multiple threads affect your calculations? Is there a race in this code?

```
   dx          Thread 0                Thread 1
    3
    2   1000 sub  $1,%dx
    2   1001 test $0,%dx
    2   1002 jgte .top
    1   1000 sub  $1,%dx
    1   1001 test $0,%dx
    1   1002 jgte .top
    0   1000 sub  $1,%dx
    0   1001 test $0,%dx
    0   1002 jgte .top
   -1   1000 sub  $1,%dx
   -1   1001 test $0,%dx
   -1   1002 jgte .top
   -1   1003 halt
    3   ----- Halt;Switch -----  ----- Halt;Switch -----
    2                            1000 sub  $1,%dx
    2                            1001 test $0,%dx
    2                            1002 jgte .top
    1                            1000 sub  $1,%dx
    1                            1001 test $0,%dx
    1                            1002 jgte .top
    0                            1000 sub  $1,%dx
    0                            1001 test $0,%dx
    0                            1002 jgte .top
   -1                            1000 sub  $1,%dx
   -1                            1001 test $0,%dx
   -1                            1002 jgte .top
   -1                            1003 halt
```

There is no race condition here regardless of the interrupt frequency: both threads have independent registers.

**Q3**. Run this: `./x86.py -p loop.s -t 2 -i 3 -r -a dx=3,dx=3 -R dx` This makes the interrupt interval small/random; use different seeds (`-s`) to see different interleavings. Does the interrupt frequency change anything?

```
dx          Thread 0                Thread 1
 3
 2   1000 sub  $1,%dx
 2   1001 test $0,%dx
 2   1002 jgte .top
 3   ------ Interrupt ------  ------ Interrupt ------
 2                            1000 sub  $1,%dx
 2                            1001 test $0,%dx
 2                            1002 jgte .top
 2   ------ Interrupt ------  ------ Interrupt ------
 1   1000 sub  $1,%dx
 1   1001 test $0,%dx
 2   ------ Interrupt ------  ------ Interrupt ------
 1                            1000 sub  $1,%dx
 1   ------ Interrupt ------  ------ Interrupt ------
 1   1002 jgte .top
 0   1000 sub  $1,%dx
 1   ------ Interrupt ------  ------ Interrupt ------
 1                            1001 test $0,%dx
 1                            1002 jgte .top
 0   ------ Interrupt ------  ------ Interrupt ------
 0   1001 test $0,%dx
 0   1002 jgte .top
-1   1000 sub  $1,%dx
 1   ------ Interrupt ------  ------ Interrupt ------
 0                            1000 sub  $1,%dx
-1   ------ Interrupt ------  ------ Interrupt ------
-1   1001 test $0,%dx
-1   1002 jgte .top
 0   ------ Interrupt ------  ------ Interrupt ------
 0                            1001 test $0,%dx
 0                            1002 jgte .top
-1   ------ Interrupt ------  ------ Interrupt ------
-1   1003 halt
 0   ----- Halt;Switch -----  ----- Halt;Switch -----
-1                            1000 sub  $1,%dx
-1                            1001 test $0,%dx
-1   ------ Interrupt ------  ------ Interrupt ------
-1                            1002 jgte .top
-1                            1003 halt
```

The interrupt frequency does not change the outcome of the program since both threads have independent registers.

**Q4**. Now, a different program, `looping-race-nolock.s`, which accesses a shared variable located at address 2000; we’ll call this variable `value`. Run it with a single thread to confirm your understanding: `./x86.py -p looping-race-nolock.s -t 1 -M 2000` What is `value` (i.e., at memory address 2000) throughout the run? Use `-c` to check.

```
$ $ ./x86.py -p looping-race-nolock.s -t 1 -M 2000 -R ax,bx
2000      ax    bx          Thread 0
   0       0     0
   0       0     0   1000 mov 2000, %ax
   0       1     0   1001 add $1, %ax
   1       1     0   1002 mov %ax, 2000
   1       1    -1   1003 sub  $1, %bx
   1       1    -1   1004 test $0, %bx
   1       1    -1   1005 jgt .top
   1       1    -1   1006 halt
   ```

**Q5**. Run with multiple iterations/threads: `./x86.py -p looping-race-nolock.s -t 2 -a bx=3 -M 2000` Why does each thread loop three times? What is final value of `value`?

* Each thread loops 3 times because both threads have independent registers and `bx=3`.
* `value=6`
* There is a race condition in this code but we did not see it happen in this execution trace because the threads did not interleave. To see it happen either `intfreq` should be set lower so that interrupts occur more often, or there should be more loop iterations to increase the number of instructions per thread.

**Q6**. Run with random interrupt intervals: `./x86.py -p looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 0` with different seeds (`-s 1`, `-s 2`, etc.) Can you tell by looking at the thread interleaving what the final value of value will be? Does the timing of the interrupt matter? Where can it safely occur? Where not? In other words, where is the critical section exactly?

```
$ ./x86.py -p looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 0 -R ax,bx -c
2000      ax    bx          Thread 0                Thread 1
   0       0     0
   0       0     0   1000 mov 2000, %ax
   0       1     0   1001 add $1, %ax
   1       1     0   1002 mov %ax, 2000
   1       1    -1   1003 sub  $1, %bx
   1       0     0   ------ Interrupt ------  ------ Interrupt ------
   1       1     0                            1000 mov 2000, %ax
   1       2     0                            1001 add $1, %ax
   2       2     0                            1002 mov %ax, 2000
   2       2    -1                            1003 sub  $1, %bx
   2       1    -1   ------ Interrupt ------  ------ Interrupt ------
   2       1    -1   1004 test $0, %bx
   2       1    -1   1005 jgt .top
   2       2    -1   ------ Interrupt ------  ------ Interrupt ------
   2       2    -1                            1004 test $0, %bx
   2       2    -1                            1005 jgt .top
   2       1    -1   ------ Interrupt ------  ------ Interrupt ------
   2       1    -1   1006 halt
   2       2    -1   ----- Halt;Switch -----  ----- Halt;Switch -----
   2       2    -1                            1006 halt

$ ./x86.py -p looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 1 -R ax,bx -c
2000      ax    bx          Thread 0                Thread 1
   0       0     0
   0       0     0   1000 mov 2000, %ax
   0       0     0   ------ Interrupt ------  ------ Interrupt ------
   0       0     0                            1000 mov 2000, %ax
   0       1     0                            1001 add $1, %ax
   1       1     0                            1002 mov %ax, 2000
   1       1    -1                            1003 sub  $1, %bx
   1       0     0   ------ Interrupt ------  ------ Interrupt ------
   1       1     0   1001 add $1, %ax
   1       1     0   1002 mov %ax, 2000
   1       1    -1   1003 sub  $1, %bx
   1       1    -1   1004 test $0, %bx
   1       1    -1   ------ Interrupt ------  ------ Interrupt ------
   1       1    -1                            1004 test $0, %bx
   1       1    -1                            1005 jgt .top
   1       1    -1   ------ Interrupt ------  ------ Interrupt ------
   1       1    -1   1005 jgt .top
   1       1    -1   1006 halt
   1       1    -1   ----- Halt;Switch -----  ----- Halt;Switch -----
   1       1    -1   ------ Interrupt ------  ------ Interrupt ------
   1       1    -1                            1006 halt
```

Interrupts can safely occur outside the critical section (code handling shared memory, see code below).

```
mov 2000, %ax  # get 'value' at address 2000
add $1, %ax    # increment it
mov %ax, 2000  # store it back
```

**Q7**. Now examine fixed interrupt intervals: `./x86.py -p looping-race-nolock.s -a bx=1 -t 2 -M 2000 -i 1` What will the final value of the shared variable `value` be? What about when you change `-i 2`, `-i 3`, etc.? For which interrupt intervals does the program give the “correct” answer?

* `-i 1`: `value=1`
* `-i 2`: `value=1`
* `-i 3`: `value=2`
* The "correct" answer (`value=2`) appears for `intfreq >= 3` because the critical section of each thread is executed in one go without being interrupted.

**Q8**. Run the same for more loops (e.g., set `-a bx=100`). What interrupt intervals (`-i`) lead to a correct outcome? Which intervals are surprising?

Interrupt intervals set to multiples of 3 lead to correct results. Three is the number of instructions in the critical section, and they are executed first.

```sh
$ for i in {1..800}; do
    echo -n "$i "
    ./x86.py -p looping-race-nolock.s -a bx=100 -t 2 -M 2000 -i "$i" -c \
        | tail -n 1
done | awk '$2 == 200 { ($1 % 3 == 0) ? type="m" : type="x"; print type, $1, $2 }'
```

**Q9**. One last program: `wait-for-me.s`. Run: `./x86.py -p wait-for-me.s -a ax=1,ax=0 -R ax -M 2000`. This sets the `%ax` register to 1 for thread 0, and 0 for thread 1, and watches `%ax` and memory location 2000. How should the code behave? How is the value at location 2000 being used by the threads? What will its final value be?

```
$ ./x86.py -p wait-for-me.s -a ax=1,ax=0 -R ax,cx -M 2000
2000    ax    cx          Thread 0                Thread 1
0       1     0
0       1     0   1000 test $1, %ax
0       1     0   1001 je .signaller
1       1     0   1006 mov  $1, 2000
1       1     0   1007 halt
1       0     0   ----- Halt;Switch -----  ----- Halt;Switch -----
1       0     0                            1000 test $1, %ax
1       0     0                            1001 je .signaller
1       0     1                            1002 mov  2000, %cx
1       0     1                            1003 test $1, %cx
1       0     1                            1004 jne .waiter
1       0     1                            1005 halt
```

Thread 1 (waiter) waits in a busy loop for thread 0 (signaller) to set the value at address 2000 to 1.

**Q10**. Now switch the inputs: `./x86.py -p wait-for-me.s -a ax=0,ax=1 -R ax -M 2000` How do the threads behave? What is thread 0 doing? How would changing the interrupt interval (e.g., `-i 1000`, or perhaps to use random intervals) change the trace outcome? Is the program efficiently using the CPU?

* Thread 0 which is executed first is now the waiter. It waits for the value at location 2000 to be set to 1. After 50 instructions in thread 0 the first interrupt occurs and thread 1 sets the value at address 2000 to 1 before halting. Finally, thread 0 tests the value at location 2000 and halts.
* The interrupt interval determines the number of instructions thread 0 will have to wait before thread 1 can finally take over and set the value at address 2000.
* Busy-waiting/spinning is a resource waste.
