Q1. We’ll start by redoing the measurements within this chapter. Use the call `gettimeofday()` to measure time within your program. How accurate is this timer? What is the smallest interval it can measure? Gain confidence in its workings, as we will need it in all subsequent questions. You can also look into other timers, such as the cycle counter available on x86 via the `rdtsc` instruction.

I used `std::chrono::steady_clock` w/ a `tick period` of 1 ns on my computer.

Q2. Now, build a simple concurrent counter and measure how long it takes to increment the counter many times as the number of threads increases. How many CPUs are available on the system you are using? Does this number impact your measurements at all?

* See `simple_counter.cpp`.
* The following plot shows the execution time of `simple_counter` and `sloppy_counter` (`threshold=1024`) with `nthreads=[1;4]` and `nloops=1000000` averaged over 100 experiments. My system has 2 cores each able to run 2 threads in parallel.

![Performance of simple vs sloppy counters](counters_plot.png)

Q3. Next, build a version of the sloppy counter. Once again, measure its performance as the number of threads varies, as well as the threshold. Do the numbers match what you see in the chapter?

* See `sloppy_counter.cpp` and the plot in the previous question.
* The following plot shows the execution time of `sloppy_counter` with `nthreads=4`, `nloops=1000000` and `threshold=[2**0;2**10]` averaged over 100 experiments.

![Sloppy counter scaling](threshold_plot.png)

Q4. Build a version of a linked list that uses hand-over-hand locking [MS04], as cited in the chapter. You should read the paper first to understand how it works, and then implement it. Measure its performance. When does a hand-over-hand list work better than a standard list as shown in the chapter?

* See `simple_list.cpp` and `hoh_list.cpp`.
* The following plot show the execution time of the lookup operation on the 10th element in a `simple_list` (coarse-grained lock) and a `hoh_list` (hand-over-hand lock) with `nthreads=4` and `nloops=1000000` averaged over 100 experiments.

![Performance of lookup in simple vs hoh locking list](lists_plot.png)

Q5. Pick your favorite interesting data structure, such as a B-tree or other slightly more interested structure. Implement it, and start with a simple locking strategy such as a single lock. Measure its performance as the number of concurrent threads increases.

Q6. Finally, think of a more interesting locking strategy for this favorite data structure of yours. Implement it, and measure its performance. How does it compare to the straightforward locking approach?
