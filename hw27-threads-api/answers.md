Q1. First build `main-race.c`. Examine the code so you can see the (hopefully obvious) data race in the code. Now run `helgrind` (by typing `valgrind --tool=helgrind main-race`) to see how it reports the race. Does it point to the right lines of code? What other information does it give to you?

It reports that the read and write at line 15 (`balance++`) from thread 1 are in conflict with the write at line 8 (`balance++`) from thread 2. These two lines are correctly reported as the ones responsible for the data race. It also reports held locks (none in our case).

Q2. What happens when you remove one of the offending lines of code? Now add a lock around one of the updates to the shared variable, and then around both. What does `helgrind` report in each of these cases?

* `helgrind` does not report any error when removing one of the offending lines. Only one thread accesses `balance` so there is no race condition.
* Adding the lock only around one of the updates does not solve the problem, since both threads can still access `balance` at the same time. `helgrind` reports when the lock was first observed in addition to the same previous race conditions.
* Adding the lock around both `balance++` lines solves the race conditions and `helgrind` does not report errors anymore.

Q3. Now let’s look at `main-deadlock.c`. Examine the code. This code has a problem known as **deadlock** (which we discuss in much more depth in a forthcoming chapter). Can you see what problem it might have?

The locks are acquired in a different order in the 2 threads, which can cause a deadlock: both thread might acquire one lock each and then be unable to proceed since both need the other lock to continue (see example below). Both threads need to acquire the locks in the same order to fix the deadlock.

```
- thread1 acquires m1
- thread2 acquires m2
- thread2 waits for m1
- thread1 waits for m2
-> deadlock
```

Q4. Now run `helgrind` on this code. What does it report?

It reports the incorrect order of acquisition of the locks.

Q5. Now run `helgrind` on `main-deadlock-global.c`. Examine the code; does it have the same problem that `main-deadlock.c` has? Should `helgrind` be reporting the same error? What does this tell you about tools like `helgrind`?

This code does not have the same problem. It adds a lock that covers the whole worker to "solve" the previous deadlock problem. It works but it's a quick-and-dirty solution that does not fix the underlying problem. `helgrind` reports that there is still a problem that can be solved with the order of lock acquisition between m1 and m2 but it does not give any warning about the `g` mutex being incorrectly used. It shows that debugging tools have their limits.

Q6. Let’s next look at `main-signal.c`. This code uses a variable (`done`) to signal that the child is done and that the parent can now continue. Why is this code inefficient? What does the parent end up spending its time doing, particularly if the child thread takes a long time to complete?

Using busy-waiting is a waste of CPU time. The parent checks the value of `done` repeatedly until the second thread sets it to 1.

Q7. Now run `helgrind` on this program. What does it report? Is the code correct?

`helgrind` reports a potential data race. There isn't one in this case.

Q8. Now look at a slightly modified version of the code, which is found in `main-signal-cv.c`. This version uses a condition variable to do the signaling (and associated lock). Why is this code preferred to the previous version? Is it correctness, or performance, or both?

This code avoids wasting resources and is less error-prone.

Q9. Once again run `helgrind` on `main-signal-cv`. Does it report any errors?

No errors reported.
