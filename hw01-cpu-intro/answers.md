> Q1. Run process-run.py with the following flags: -l 5:100,5:100. What should the CPU utilization be (e.g., the percent of time the CPU is in use?) Why do you know this? Use the -c and -p flags to see if you were right.

The CPU is used 100% of the time. The first process runs 5 instructions with 100% chance of them being CPU instructions (and 0% chance of issuing an I/O). Thus CPU utilization is 100% for this first process. Then the second process does the same thing and also uses the CPU 100% of the time.

> Q2. Now run with these flags: ./process-run.py -l 4:100,1:0. These flags specify one process with 4 instructions (all to use the CPU), and one that simply issues an I/O and waits for it to be done. How long does it take to complete both processes? Use -c and -p to find out if you were right.

First process will execute 4 CPU instructions, finish, second process will start, execute one cpu instruction to issue an I/O, wait 5 ticks (default IO_LENGTH), then execute one final cpu instruction to conclude io. In total, the processes will complete after 11 steps.

> Q3. Switch the order of the processes: -l 1:0,4:100. What happens now? Does switching the order matter? Why? (As always, use -c and -p to see if you were right)

The process will complete after 7 steps. While p0 is waiting for I/O to finish, the OS can run p1's 4 CPU instructions.

> Q4. We’ll now explore some of the other flags. One important flag is -S, which determines how the system reacts when a process issues an I/O. With the flag set to SWITCH ON END, the system will NOT switch to another process while one is doing I/O, instead waiting until the process is completely finished. What happens when you run the following two processes (-l 1:0,4:100 -c -S SWITCH ON END), one doing I/O and the other doing CPU work?

Same scenario as in Q2. The OS does not switch to p1 while p0 is WAITING for IO to finish.

> Q5. Now, run the same processes, but with the switching behavior set to switch to another process whenever one is WAITING for I/O (-l 1:0,4:100 -c -S SWITCH ON IO). What happens now? Use -c and -p to confirm that you are right.

Same scenario as in Q3.

> Q6. One other important behavior is what to do when an I/O completes. With -I IO RUN LATER, when an I/O completes, the process that issued it is not necessarily run right away; rather, whatever was running at the time keeps running. What happens when you run this combination of processes? (Run ./process-run.py -l 3:0,5:100,5:100,5:100 -S SWITCH ON IO -I IO RUN LATER -c -p) Are system resources being effectively utilized?

System resources aren't effectively utilized. p2 and p3 could have executed their cpu instructions while p0 waited for its 2nd and 3rd I/O requests to finish.

> Q7. Now run the same processes, but with -I IO RUN IMMEDIATE set, which immediately runs the process that issued the I/O. How does this behavior differ? Why might running a process that just completed an I/O again be a good idea?

Now the CPU is used to execute p1, p2 and p3 instructions while p0 waits for its
3 successive I/O requests to complete.

Running a process that just completed an I/O again might be a good idea because the process might issue an I/O request again just after. It also allows the process to clean-up resources as soon as possible.

> Q8. Now run with some randomly generated processes: -s 1 -l 3:50,3:50 or -s 2 -l 3:50,3:50 or -s 3 -l 3:50,3:50. See if you can predict how the trace will turn out. What happens when you use the flag -I IO RUN IMMEDIATE vs. -I IO RUN LATER? What happens when you use -S SWITCH ON IO vs. -S SWITCH ON END?

* -S SWITCH_ON_IO
    * -I IO_RUN_IMMEDIATE
        * -s1: 15
        * -s2: 16
        * -s3: 17
    * -I IO_RUN_LATER
        * -s1: 15: same
        * -s2: 16: same
        * -s3: 18: worse
* -S SWITCH_ON_END
    * -s1: 18: worse
    * -s2: 30: worse
    * -s3: 24: worse

SWITCH_ON_END is always worse than SWITCH_ON_IO because processes are run in a batch (one after the other). IO_DONE_BEHAVIOR might depend on the case, but IO_RUN_IMMEDIATE seems better in general as stated in Q7.
