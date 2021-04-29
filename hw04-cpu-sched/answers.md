> 1. Compute the response time and turnaround time when running three jobs of length 200 with the SJF and FIFO schedulers.

We have the same results for SJF and FIFO.
* `T_arrival = 0`
* `T_response = T_firstrun - T_arrival = T_firstrun`
* `T_turnaround = T_completion - T_arrival = T_completion`
* `T_response_avg = (0 + 200 + 400) / 3 = 200`
* `T_turnaround_avg = (200 + 400 + 600) / 3 = 400`

```sh
$ ./scheduler.py -p SJF -l 200,200,200 -c
  Average -- Response: 200.00  Turnaround 400.00  Wait 200.00
$ ./scheduler.py -p FIFO -l 200,200,200 -c
  Average -- Response: 200.00  Turnaround 400.00  Wait 200.00
```

> 2. Now do the same but with jobs of different lengths: 100, 200, and 300.

Results are the same again for SJF and FIFO.
* `T_response_avg = (0 + 100 + 300) / 3 = 133.33`
* `T_turnaround_avg = (100 + 300 + 600) / 3 = 333.33`

```sh
$ ./scheduler.py -p SJF -l 100,200,300 -c
  Average -- Response: 133.33  Turnaround 333.33  Wait 133.33
$ ./scheduler.py -p FIFO -l 100,200,300 -c
  Average -- Response: 133.33  Turnaround 333.33  Wait 133.33
```

> 3. Now do the same, but also with the RR scheduler and a time-slice of 1.

* RR `-q 1 -l 200,200,200`
    * `Travg = (0 + 1 + 2) / 3 = 1`
    * `Ttavg = (598 + 599 + 600) / 3 = 599`
* RR `-q 1 -l 100,200,300`
    * `Travg = (0 + 1 + 2) / 3 = 1`
    * `Tt[n]   = sum( jobs[i].length for i in range(n) )`
    * `Tt[n-1] = Tt[n]   - jobs[0].length     - 1`
    * `Tt[j]   = Tt[j-1] - jobs[n-j-1].length - 1`
    * `Ttavg = (600 + 499 + 298) / 3 = 465.67`

```sh
$ ./scheduler.py -p RR -q 1 -l 200,200,200 -c
  Average -- Response: 1.00  Turnaround 599.00  Wait 399.00
$ ./scheduler.py -p RR -q 1 -l 100,200,300 -c
  Average -- Response: 1.00  Turnaround 465.67  Wait 265.67
```

> 4. For what types of workloads does SJF deliver the same turnaround times as FIFO?

Jobs with the same arrival time and with ascending or equal length have the same turnaround time with SJF and FIFO.

```bash
diff_sjf_fifo() {
    diff <(./scheduler.py -p SJF  -l "$1" -c | awk '/Final statistics:/,EOF') \
         <(./scheduler.py -p FIFO -l "$1" -c | awk '/Final statistics:/,EOF')
}

$ diff_sjf_fifo 10,20,30 # no difference
$ diff_sjf_fifo 20,20,20 # no difference
$ diff_sjf_fifo 20,10,30 # difference
-  Average -- Response: 13.33  Turnaround 33.33  Wait 13.33
+  Average -- Response: 16.67  Turnaround 36.67  Wait 16.67
$ diff_sjf_fifo 30,20,10 # difference
-  Average -- Response: 13.33  Turnaround 33.33  Wait 13.33
+  Average -- Response: 26.67  Turnaround 46.67  Wait 26.67
```

> 5. For what types of workloads and quantum lengths does SJF deliver the same response times as RR?

For SJF and RR to deliver the same response times the following criteria need to be fulfilled:
* jobs need to have the same arrival time
* jobs need to have equal or ascending length
* quantum length needs to be superior or equal to the penultimate job's length

```bash
diff_sjf_rr() {
    diff <(./scheduler.py -p SJF -q "$1" -l "$2" -c \
           | awk '/Final statistics:/,EOF') \
         <(./scheduler.py -p RR  -q "$1" -l "$2" -c \
           | awk '/Final statistics:/,EOF')
}

$ diff_sjf_rr 30 30,30,30,30,30 # no difference
$ diff_sjf_rr 99 10,34,35,36,37,73,74,98,101 # no difference
$ diff_sjf_rr 98 10,34,35,36,37,73,74,98,101 # no difference
$ diff_sjf_rr 97 10,34,35,36,37,73,74,98,101 # difference
...
```

> 6. What happens to response time with SJF as job lengths increase? Can you use the simulator to demonstrate the trend?

With FIFO, response time **accumulates** job lengths. With SJF, response time accumulates **sorted** job lengths. In both cases response time increases as job lengths increase.

```sh
$ ./scheduler.py -p SJF -l 1,30,70 -c # Tr0=0, Tr1=1, Tr2=31
$ ./scheduler.py -p SJF -l 15,30,70 -c # Tr0=0, Tr1=15, Tr2=45
$ ./scheduler.py -p SJF -l 40,30,70 -c # Tr1=0, Tr0=30, Tr2=70
```

See [response.py](response.py) for implementations of `FIFO_Tr(jobs)` and `SJF_Tr(jobs)`.

> 7. What happens to response time with RR as quantum lengths increase? Can you write an equation that gives the worst-case response time, given N jobs?

As quantum lengths increase, response time increases. Quantum length determines the length of a time slice. The longer it is, the longer the period between context switches will be, the longer the wait before the first run of each job will be.

Given `N` jobs, the worst case response time `q * (N - 1)` happens when `jobs[0:N-1]` all have the same length also equal to `q`.

See [response.py](response.py) for implementations of `RR_Tr(jobs)`.
