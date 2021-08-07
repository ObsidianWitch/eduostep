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

Higher seek rates allow to avoid missing some sectors during seeks and thus reduce rotation time on top of reducing seek time.

Q3. Do the same requests above, but change the rotation rate: `-R 0.1`, `-R 0.5`, `-R 0.01`. How do the times change?

Rotation and transfer times are inversely proportional to the rotation rate.

Q4. FIFO is not always best, e.g., with the request stream `-a 7,30,8`, what order should the requests be processed in? Run the shortest seek-time first (SSTF) scheduler (`-p SSTF`) on this workload; how long should it take (seek, rotation, transfer) for each request to be served?

```sh
# FIFO: 7->30->8
# SSTF: 7->8->30

$ ./disk.py -a 7,30,8 -p SSTF
# 7:  seek=0  rotate=15            transfer=30 total=45
# 8:  seek=0  rotate=0             transfer=30 total=30
# 30: seek=80 rotate=(9*30)-80=190 transfer=30 total=300
# t:  seek=80 rotate=205           transfer=90 total=375
```

Q5. Now use the shortest access-time first (SATF) scheduler (`-p SATF`). Does it make any difference for `-a 7,30,8` workload? Find a set of requests where SATF outperforms SSTF; more generally, when is SATF better than SSTF?

* same results for the SATF and and SSTF policies on the `-a 7,30,8` workload
* example of workload where SATF outperforms SSTF: `-a 6,20,33`. SATF can take seek speed and rotation speed into account to decide whether it's beneficial to go to a farther track instead of the nearest one.
* SATF performs better than SSTF when seeking is faster than rotation.

Q6. Here is a request stream to try: `-a 10,11,12,13`. What goes poorly when it runs? Try adding track skew to address this problem (-o skew). Given the default seek rate, what should the skew be to maximize performance? What about for different seek rates (e.g., `-S 2`, `-S 4`)? In general, could you write a formula to figure out the skew?

* the head must wait almost an entire rotation because the requested block (12) has been passed during seek even though the workload is sequential
* `./disk.py -a 10,11,12,13 -o 2 -c`
* ```py
skew * blocks_angle / rotation_speed > track_width / seek_speed
skew = math.ceil((track_width * rotation_speed) / (seek_speed * blocks_angle))
```

Q7. Specify a disk with different density per zone, e.g., `-z 10,20,30`, which specifies the angular difference between blocks on the outer, middle, and inner tracks. Run some random requests (e.g., `-a -1 -A 5,-1,0`, which specifies that random requests should be used via the `-a -1` flag and that five requests ranging from 0 to the max be generated), and compute the seek, rotation, and transfer times. Use different random seeds. What is the bandwidth (in sectors per unit time) on the outer, middle, and inner tracks?

```
angular_speed = 1 degree/time_unit
z1 = 10 degrees/sector
z2 = 20 degrees/sector
z3 = 30 degrees/sector

z1_transfer_speed = angular_speed / z1 = 0.100 sectors/time_unit
z2_transfer_speed = 0.050 sectors/time_unit
z3_transfer_speed = 0.033 sectors/time_unit
```

Q8. A scheduling window determines how many requests the disk can examine at once. Generate random workloads (e.g., `-A 1000,-1,0`, with different seeds) and see how long the SATF scheduler takes when the scheduling window is changed from 1 up to the number of requests. How big of a window is needed to maximize performance? Hint: use the `-c` flag and don’t turn on graphics (`-G`) to run these quickly. When the scheduling window is set to 1, does it matter which policy you are using?

```sh
for window in 1 {10..1000..10}; do
    ./disk.py -A '1000,-1,0' -p SATF -w "$window" -c \
        | awk '/window/{input=$3} /TOTALS/{print "Window:"input, $5}'
done
```

* For this example (`seed=0`), max performance is reached w/ a `window≈170`. The bigger the window is, the more the scheduler will have to wait before servicing requests which will add latency. Increasing the window will also increase the number of requests to sort, which might also contribute to the latency.
* A scheduling window of 1 is equivalent to using FIFO regardless of the chosen policy.

Q9. Create a series of requests to starve a particular request, assuming an SATF policy. Given that sequence, how does it perform if you use a bounded SATF (BSATF) scheduling approach? In this approach, you specify the scheduling window (e.g., `-w 4`); the scheduler only moves onto the next window of requests when all requests in the current window have been serviced. Does this solve starvation? How does it perform, as compared to SATF? In general, how should a disk make this trade-off between performance and starvation avoidance?

* `./disk.py -p SATF -a 7,19,8,9,10,11,0,1,2,3,4,5,6,7,8 -c`
* The total time increases w/ BSATF and a window of 4 blocks, but the request for block 19 does not starve.

Q10. All the scheduling policies we have looked at thus far are greedy; they pick the next best option instead of looking for an optimal schedule. Can you find a set of requests in which greedy is not optimal?

```sh
$ ./disk.py -a 20,34,35 -p FIFO -c # total=525
$ ./disk.py -a 20,34,35 -p SSTF -c # total=495
$ ./disk.py -a 20,34,35 -p SATF -c # total=495

$ ./disk.py -a 34,35,20 -p FIFO -c # optimal order, total=435
```
