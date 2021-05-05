> 1. Run a few randomly-generated problems with just two jobs and two queues; compute the MLFQ execution trace for each. Make your life easier by limiting the length of each job and turning off I/Os.

```sh
$ ./mlfq.py --numJobs=2 --numQueues=2 --maxio=0 --seed=0
OPTIONS jobs 2
OPTIONS queues 2
OPTIONS allotments for queue  1 is   1
OPTIONS quantum length for queue  1 is  10
OPTIONS allotments for queue  0 is   1
OPTIONS quantum length for queue  0 is  10
OPTIONS boost 0
OPTIONS ioTime 5
OPTIONS stayAfterIO False
OPTIONS iobump False

Job  0: startTime   0 - runTime   8 - ioFreq   0
Job  1: startTime   0 - runTime   4 - ioFreq   0

* job 0: time:10/84 allotment:10/10 priority:1->0
* job 1: time:10/42 allotment:10/10 priority:1->0
* job 0: time:20/84 allotment:10/10 priority:0
* job 1: time:20/42 allotment:10/10 priority:0
* job 0: time:30/84 allotment:10/10 priority:0
* job 1: time:30/42 allotment:10/10 priority:0
* job 0: time:40/84 allotment:10/10 priority:0
* job 1: time:40/42 allotment:10/10 priority:0
* job 0: time:50/84 allotment:10/10 priority:0
* job 1: time:42/42 allotment:02/10 priority:0->finished
* job 0: time:60/84 allotment:10/10 priority:0
* job 0: time:70/84 allotment:10/10 priority:0
* job 0: time:80/84 allotment:10/10 priority:0
* job 0: time:84/84 allotment:04/10 priority:0->finished
```

> 2. How would you run the scheduler to reproduce each of the examples in the chapter?

```sh
# figure 8.2: Long-running Job Over Time
$ ./mlfq.py --jlist=0,200,0 -c

# figure 8.3: Along Came An Interactive Job
$ ./mlfq.py --jlist=0,180,0:100,20,0 -c

# figure 8.4: A Mixed I/O-intensive and CPU-intensive Workload
$ ./mlfq.py --stay --jlist=0,175,0:50,25,1 -c

# figure 8.5.a: Without Priority Boost
$ ./mlfq.py --iotime=2 --stay --jlist=0,175,0:100,50,2:100,50,2 -c

# figure 8.5.b: With Priority Boost
$ ./mlfq.py --boost=50 --iotime=2 --stay --jlist=0,175,0:100,50,2:100,50,2 -c

# figure 8.6.a: Without Gaming Tolerance
$ ./mlfq.py --iotime=1 --stay --jlist=0,175,0:80,90,9 -c

# figure 8.6.b: With Gaming Tolerance
$ ./mlfq.py --iotime=1 --jlist=0,175,0:80,90,9 -c

# figure 8.7: Lower Priority, Longer Quanta
$ ./mlfq.py --allotment=2 --quantumList=10,20,40 --jlist=0,140,0:0,140,0 -c
```

> 3. How would you configure the scheduler parameters to behave just like a round-robin scheduler?

Jobs on the same queue are scheduled with round-robin. Configuring MLFQ to only use one queue makes it act like round-robin.

> 4. Craft a workload with two jobs and scheduler parameters so that one job takes advantage of the older Rules 4a and 4b (turned on with the -S flag) to game the scheduler and obtain 99% of the CPU over a particular time interval.

```sh
$ ./mlfq.py --quantum=100 --iotime=1 --stay --jlist=0,200,0:0,200,99 -c
```

Job 1 obtains 99% of the CPU with `t in [100;302]` by setting `quantum=100`, `iotime=1` and `j1.iofreq=99`. The first time job 1 runs, it will execute 99 CPU instructions, avoid dropping its priority by doing 1 I/O request of length 1, and repeat these steps until it has finished.

```
j0.iofreq=0 or j0.iofreq >= quantum
quantum > j1.iofreq
j1.cpuusage = j1.length / (j1.completion - j1.firstrun)
            = j1.length / j1.runtime
            = j1.length / (j1.length + j1.length * iotime / j1.iofreq)
            = j1.iofreq / (j1.iofreq + iotime)
```

> 5. Given a system with a quantum length of 10 ms in its highest queue, how often would you have to boost jobs back to the highest priority level (with the -B flag) in order to guarantee that a single long-running (and potentially-starving) job gets at least 5% of the CPU?

```sh
$ ./mlfq.py --boost=250 --iotime=2 --stay --jlist=0,500,0:0,500,2:0,500,2 -c
```

Over a particular time interval (i.e. [0;1000[), we want our long-running job to get at least 5% = 50ms of the cpu.

```
objective = interval_length * 5% (= 50)
divisor = (objective / quantum) - 1 (= 4)
boost = interval_length / divisor (= 250)
```

> 6. One question that arises in scheduling is which end of a queue to add a job that just finished I/O; the -I flag changes this behavior for this scheduling simulator. Play around with some workloads and see if you can see the effect of this flag.

```sh
$ mlfq -h
-I, --iobump          if specified, jobs that finished I/O move immediately
                      to front of current queue

$ ./mlfq.py --numQueues=1 --jlist=0,50,0:0,25,13 -c
$ ./mlfq.py --numQueues=1 --iobump --jlist=0,50,0:0,25,13 -c
```

> Bonus. Implement a tool to plot the results of `./mlfq.py`.

```sh
# display figure in a new window
$ ./mlfq.py --jlist=0,200,0 -c | ./plot.py

# display figure in current kitty terminal
$ ./mlfq.py --jlist=0,200,0 -c | ./plot.py | kitty +kitten icat

# save figure to png file
$ ./mlfq.py --jlist=0,200,0 -c | ./plot.py > out.png
```
