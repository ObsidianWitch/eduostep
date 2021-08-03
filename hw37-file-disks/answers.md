Q1. Compute the seek, rotation, and transfer times for the following sets of requests: `-a 0`, `-a 6`, `-a 30`, `-a 7,30,8`, and finally `-a 10,11,12,13`.

```sh
$ ./disk.py -a 0
# 0: seek=0 rotate=15+(5*30)=165 transfer=30

$ ./disk.py -a 6
# 6: seek=0 rotate=15+(11*30)=345 transfer=30

$ ./disk.py -a 30
# 30: seek=2*40=80 rotate=15+(11*30)-seek=265 transfer=30

$ ./disk.py -a 7,30,8
# 7:  seek=0   rotate=15             transfer=30 total=45
# 30: seek=80  rotate=(10*30)-80=220 transfer=30 total=330
# 8:  seek=80  rotate=360+30-80=310  transfer=30 total=420
# t:  seek=160 rotate=545            transfer=90 total=795

$ ./disk.py -a 10,11,12,13
# 10: seek=0  rotate=15+(3*30)=105 transfer=30  total=135
# 11: seek=0  rotate=0             transfer=30  total=30
# 12: seek=40 rotate=360-40=320    transfer=30  total=390
# 13: seek=0  rotate=0             transfer=30  total=30
# t:  seek=40 rotate=425           transfer=120 total=585
```

Q2. Do the same requests above, but change the seek rate to different values: `-S 2`, `-S 4`, `-S 8`, `-S 10`, `-S 40`, `-S 0.1`. How do the times change?

Higher seek rates allow to avoid missing some sectors (different tracks, non-aligned, non-adjacent) during seeks and thus reduce rotation time on top of reducing seek time.

Q3. Do the same requests above, but change the rotation rate: `-R 0.1`, `-R 0.5`, `-R 0.01`. How do the times change?

Rotation and transfer times are inversely proportional to the rotation rate.

Q4. FIFO is not always best, e.g., with the request stream `-a 7,30,8`, what order should the requests be processed in? Run the shortest seek-time first (SSTF) scheduler (`-p SSTF`) on this workload; how long should it take (seek, rotation, transfer) for each request to be served?

```sh
$ ./disk.py -a 7,30,8 -p SSTF
# 7:  seek=0  rotate=15            transfer=30 total=45
# 8:  seek=0  rotate=0             transfer=30 total=30
# 30: seek=80 rotate=(9*30)-80=190 transfer=30 total=300
# t:  seek=80 rotate=205           transfer=90 total=375
```

Q5. Now use the shortest access-time first (SATF) scheduler (`-p SATF`). Does it make any difference for `-a 7,30,8` workload? Find a set of requests where SATF outperforms SSTF; more generally, when is SATF better than SSTF?

Q6. Here is a request stream to try: `-a 10,11,12,13`. What goes poorly when it runs? Try adding track skew to address this problem (-o skew). Given the default seek rate, what should the skew be to maximize performance? What about for different seek rates (e.g., `-S 2`, `-S 4`)? In general, could you write a formula to figure out the skew?

Q7. Specify a disk with different density per zone, e.g., `-z 10,20,30`, which specifies the angular difference between blocks on the outer, middle, and inner tracks. Run some random requests (e.g., `-a -1 -A 5,-1,0`, which specifies that random requests should be used via the `-a -1` flag and that five requests ranging from 0 to the max be generated), and compute the seek, rotation, and transfer times. Use different random seeds. What is the bandwidth (in sectors per unit time) on the outer, middle, and inner tracks?

Q8. A scheduling window determines how many requests the disk can examine at once. Generate random workloads (e.g., `-A 1000,-1,0`, with different seeds) and see how long the SATF scheduler takes when the scheduling window is changed from 1 up to the number of requests. How big of a window is needed to maximize performance? Hint: use the `-c` flag and don’t turn on graphics (`-G`) to run these quickly. When the scheduling window is set to 1, does it matter which policy you are using?

Q9. Create a series of requests to starve a particular request, assuming an SATF policy. Given that sequence, how does it perform if you use a bounded SATF (BSATF) scheduling approach? In this approach, you specify the scheduling window (e.g., `-w 4`); the scheduler only moves onto the next window of requests when all requests in the current window have been serviced. Does this solve starvation? How does it perform, as compared to SATF? In general, how should a disk make this trade-off between performance and starvation avoidance?

Q10. All the scheduling policies we have looked at thus far are greedy; they pick the next best option instead of looking for an optimal schedule. Can you find a set of requests in which greedy is not optimal?
