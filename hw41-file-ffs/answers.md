Q1. Examine the file `in.largefile`, and then run the simulator with flag `-f in.largefile` and `-L 4`. The latter sets the large-file exception to 4 blocks. What will the resulting allocation look like? Run with `-c` to check.

```
$ ./ffs.py -f in.largefile -L 4
[...]
group inodes    data
    0 /a-------- /aaaa----- --------------------
    1 ---------- aaaa------ --------------------
    2 ---------- aaaa------ --------------------
    3 ---------- aaaa------ --------------------
    4 ---------- aaaa------ --------------------
    5 ---------- aaaa------ --------------------
    6 ---------- aaaa------ --------------------
    7 ---------- aaaa------ --------------------
    8 ---------- aaaa------ --------------------
    9 ---------- aaaa------ --------------------
```

Q2. Now run with `-L 30`. What do you expect to see? Once again, turn on `-c` to see if you were right. You can also use `-S` to see exactly which blocks were allocated to the file `/a`.

```
$ ./ffs.py -f in.largefile -L 30
[...]
group inodes    data
    0 /a-------- /aaaaaaaaa aaaaaaaaaa aaaaaaaaaa
    1 ---------- aaaaaaaaaa a--------- ----------
    2 ---------- ---------- ---------- ----------
    3 ---------- ---------- ---------- ----------
    4 ---------- ---------- ---------- ----------
    5 ---------- ---------- ---------- ----------
    6 ---------- ---------- ---------- ----------
    7 ---------- ---------- ---------- ----------
    8 ---------- ---------- ---------- ----------
    9 ---------- ---------- ---------- ----------
```

Q3. Now we will compute some statistics about the file. The first is something we call *filespan*, which is the max distance between any two data blocks of the file or between the inode and any data block. Calculate the filespan of `/a`. Run `ffs.py -f in.largefile -L 4 -T -c` to see what it is. Do the same with `-L 100`. What difference do you expect in filespan as the large-file exception parameter changes from low values to high values?

```
$ ./ffs.py -f in.largefile -L 4 -T -c
[...]
group inodes    data
    0 /a-------- /aaaa----- ---------- ----------
    1 ---------- aaaa------ ---------- ----------
    2 ---------- aaaa------ ---------- ----------
    3 ---------- aaaa------ ---------- ----------
    4 ---------- aaaa------ ---------- ----------
    5 ---------- aaaa------ ---------- ----------
    6 ---------- aaaa------ ---------- ----------
    7 ---------- aaaa------ ---------- ----------
    8 ---------- aaaa------ ---------- ----------
    9 ---------- aaaa------ ---------- ----------

span: files
  file:         /a  filespan: 372 (= 8 + (10 * 3) + (10 * 4 * 8) + 10 + 4)
               avg  filespan: 372.00

span: directories
  dir:           /  dirspan: 373 (= 9 + (10 * 3) + (10 * 4 * 8) + 10 + 4)
               avg  dirspan: 373.00
```

```
$ ./ffs.py -f in.largefile -L 100 -T -c
[...]
group inodes    data
    0 /a-------- /aaaaaaaaa aaaaaaaaaa aaaaaaaaaa
    1 ---------- aaaaaaaaaa a--------- ----------
    2 ---------- ---------- ---------- ----------
    3 ---------- ---------- ---------- ----------
    4 ---------- ---------- ---------- ----------
    5 ---------- ---------- ---------- ----------
    6 ---------- ---------- ---------- ----------
    7 ---------- ---------- ---------- ----------
    8 ---------- ---------- ---------- ----------
    9 ---------- ---------- ---------- ----------

span: files
  file:         /a  filespan:  59
               avg  filespan:  59.00

span: directories
  dir:           /  dirspan:  60
               avg  dirspan:  60.00
```

Let's assume we have allocated both large files and small files in our file system. I expect increasing the exception parameter will reduce the filespan of large files, and increase the filespan of small files.

Q4. Now let’s look at a new input file, `in.manyfiles`. How do you think the FFS policy will lay these files out across groups? (you can run with `-v` to see what files and directories are created, or just `cat in.manyfiles`). Run the simulator with `-c` to see if you were right.

```
$ ./ffs.py -f in.manyfiles
[...]
group inodes    data
    0 /abcdefghi /aabbccdde effgghhii- ----------
    1 jlmnopqrC- jlmnopqrCC C--------- ----------
    2 tuvwxyzAB- tuuuvvvwww xxxyyyzzzA AABBB-----
    3 ---------- ---------- ---------- ----------
    4 ---------- ---------- ---------- ----------
    5 ---------- ---------- ---------- ----------
    6 ---------- ---------- ---------- ----------
    7 ---------- ---------- ---------- ----------
    8 ---------- ---------- ---------- ----------
    9 ---------- ---------- ---------- ----------
```

Q5. A metric to evaluate FFS is called *dirspan*. This metric calculates the spread of files within a particular directory, specifically the max distance between the inodes and data blocks of all files in the directory and the inode and data block of the directory itself. Run with `in.manyfiles` and the `-T` flag, and calculate the dirspan of the three directories. Run with `-c` to check. How good of a job does FFS do in minimizing dirspan?

```
$ ./ffs.py -f in.manyfiles -c -T
[...]
group inodes    data
    0 /abcdefghi /aabbccdde effgghhii- ----------
    1 jlmnopqrC- jlmnopqrCC C--------- ----------
    2 tuvwxyzAB- tuuuvvvwww xxxyyyzzzA AABBB-----
    3 ---------- ---------- ---------- ----------
    4 ---------- ---------- ---------- ----------
    5 ---------- ---------- ---------- ----------
    6 ---------- ---------- ---------- ----------
    7 ---------- ---------- ---------- ----------
    8 ---------- ---------- ---------- ----------
    9 ---------- ---------- ---------- ----------

span: files
  file:         /a  filespan:  11
  file:         /b  filespan:  12
  file:         /c  filespan:  13
  file:         /d  filespan:  14
  file:         /e  filespan:  15
  file:         /f  filespan:  16
  file:         /g  filespan:  17
  file:         /h  filespan:  18
  file:         /i  filespan:  19
  file:       /t/u  filespan:  12
  file:       /j/l  filespan:  10
  file:       /t/v  filespan:  14
  file:       /j/m  filespan:  10
  file:       /t/w  filespan:  16
  file:       /j/n  filespan:  10
  file:       /t/x  filespan:  18
  file:       /j/o  filespan:  10
  file:       /t/y  filespan:  20
  file:       /j/p  filespan:  10
  file:       /t/z  filespan:  22
  file:       /j/q  filespan:  10
  file:       /t/A  filespan:  24
  file:       /j/r  filespan:  10
  file:       /t/B  filespan:  26
  file:       /j/C  filespan:  12
               avg  filespan:  14.76

span: directories
  dir:           /  dirspan:  28
  dir:          /j  dirspan:  20
  dir:          /t  dirspan:  34
               avg  dirspan:  27.33
```

FFS tries to place files' blocks (inode & data) in the same group as their directory to minimize dirspan.

```
$ ./ffs.py -f in.manyfiles -c -T
[...]
group inodes    data
    0 /abcdefghi /aabbccdde effgghhii- ----------
    1 jlmnopqrC- jlmnopqrCC C--------- ----------
    2 tuvwxyzAB- tuuuvvvwww xxxyyyzzzA AABBB-----
    3 ---------- ---------- ---------- ----------
    4 ---------- ---------- ---------- ----------
    5 ---------- ---------- ---------- ----------
    6 ---------- ---------- ---------- ----------
    7 ---------- ---------- ---------- ----------
    8 ---------- ---------- ---------- ----------
    9 ---------- ---------- ---------- ----------

span: files
  file:         /a  filespan:  11
  file:         /b  filespan:  12
  file:         /c  filespan:  13
  file:         /d  filespan:  14
  file:         /e  filespan:  15
  file:         /f  filespan:  16
  file:         /g  filespan:  17
  file:         /h  filespan:  18
  file:         /i  filespan:  19
  file:       /t/u  filespan:  12
  file:       /j/l  filespan:  10
  file:       /t/v  filespan:  14
  file:       /j/m  filespan:  10
  file:       /t/w  filespan:  16
  file:       /j/n  filespan:  10
  file:       /t/x  filespan:  18
  file:       /j/o  filespan:  10
  file:       /t/y  filespan:  20
  file:       /j/p  filespan:  10
  file:       /t/z  filespan:  22
  file:       /j/q  filespan:  10
  file:       /t/A  filespan:  24
  file:       /j/r  filespan:  10
  file:       /t/B  filespan:  26
  file:       /j/C  filespan:  12
               avg  filespan:  14.76

span: directories
  dir:           /  dirspan:  28
  dir:          /j  dirspan:  20
  dir:          /t  dirspan:  34
               avg  dirspan:  27.33
```

Q6. Now change the size of the inode table per group to 5 (`-i 5`). How do you think this will change the layout of the files? Run with `-c` to see if you were right. How does it affect the dirspan?

```
$ ./ffs.py -f in.manyfiles -i 5 -c -T
[...]
group inodedata
    0 /abcd /aabbccdd- ---------- ----------
    1 efghi eeffgghhii ---------- ----------
    2 jlmno jlmno----- ---------- ----------
    3 tuvwx tuuuvvvwww xxx------- ----------
    4 ypzqA yyypzzzqAA A--------- ----------
    5 rBC-- rBBBCCC--- ---------- ----------
    6 ----- ---------- ---------- ----------
    7 ----- ---------- ---------- ----------
    8 ----- ---------- ---------- ----------
    9 ----- ---------- ---------- ----------

span: files
  file:         /a  filespan:   6
  file:         /b  filespan:   7
  file:         /c  filespan:   8
  file:         /d  filespan:   9
  file:         /e  filespan:   6
  file:         /f  filespan:   7
  file:         /g  filespan:   8
  file:         /h  filespan:   9
  file:         /i  filespan:  10
  file:       /t/u  filespan:   7
  file:       /j/l  filespan:   5
  file:       /t/v  filespan:   9
  file:       /j/m  filespan:   5
  file:       /t/w  filespan:  11
  file:       /j/n  filespan:   5
  file:       /t/x  filespan:  13
  file:       /j/o  filespan:   5
  file:       /t/y  filespan:   7
  file:       /j/p  filespan:   7
  file:       /t/z  filespan:   9
  file:       /j/q  filespan:   9
  file:       /t/A  filespan:  11
  file:       /j/r  filespan:   5
  file:       /t/B  filespan:   7
  file:       /j/C  filespan:   9
               avg  filespan:   7.76

span: directories
  dir:           /  dirspan:  49
  dir:          /j  dirspan: 116
  dir:          /t  dirspan:  78
               avg  dirspan:  81.00
```

Reducing the number of inodes per group increases the dirspan of directories. File inodes that can't be allocated in the same group as their directory are allocated in the next group with free inodes. And the data blocks are placed in the same group as the file's inode (if possible). Thus it increases the distance between a directory inode and the farthest file data block.

Q7. Which group should FFS place inode of a new directory in? The default (simulator) policy looks for the group with the most free inodes. A different policy looks for a set of groups with the most free inodes. For example, if you run with `-A 2`, when allocating a new directory, the simulator will look at groups in pairs and pick the best pair for the allocation. Run `./ffs.py -f in.manyfiles -I 5 -A 2 -c` to see how allocation changes with this strategy. How does it affect dirspan? Why might this policy be good?

Q8. One last policy change we will explore relates to file fragmentation. Run `./ffs.py -f in.fragmented -v` and see if you can predict how the files that remain are allocated. Run with `-c` to confirm your answer. What is interesting about the data layout of file `/i`? Why is it problematic?

Q9. A new policy, which we call contiguous allocation (`-C`), tries to ensure that each file is allocated contiguously. Specifically, with `-C n`, the file system tries to ensure that n contiguous blocks are free within a group before allocating a block. Run `./ffs.py -f in.fragmented -v -C 2 -c` to see the difference. How does layout change as the parameter passed to `-C` increases? Finally, how does `-C` affect filespan and dirspan?
