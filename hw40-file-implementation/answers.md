Q1. Run the simulator with some different random seeds (say 17, 18, 19, 20), and see if you can figure out which operations must have taken place between each state change.

```sh
$ ./vsfs.py -s 17
# Initial state
#
# inode bitmap  10000000
# inodes       [d a:0 r:2][][][][][][][]
# data bitmap   10000000
# data         [(.,0) (..,0)][][][][][][][]
#
# mkdir("/u");
#
# inode bitmap  11000000
# inodes       [d a:0 r:3][d a:1 r:2][][][][][][]
# data bitmap   11000000
# data         [(.,0) (..,0) (u,1)][(.,1) (..,0)][][][][][][]
#
# creat("/a");
#
# inode bitmap  11100000
# inodes       [d a:0 r:3][d a:1 r:2][f a:-1 r:1][][][][][]
# data bitmap   11000000
# data         [(.,0) (..,0) (u,1) (a,2)][(.,1) (..,0)][][][][][][]
#
# unlink("/a");
#
# inode bitmap  11000000
# inodes       [d a:0 r:3][d a:1 r:2][][][][][][]
# data bitmap   11000000
# data         [(.,0) (..,0) (u,1)][(.,1) (..,0)][][][][][][]
#
# mkdir("/z");
#
# inode bitmap  11100000
# inodes       [d a:0 r:4][d a:1 r:2][d a:2 r:2][][][][][]
# data bitmap   11100000
# data         [(.,0) (..,0) (u,1) (z,2)][(.,1) (..,0)][(.,2) (..,0)][][][][][]
#
# mkdir("/s");
#
# inode bitmap  11110000
# inodes       [d a:0 r:5][d a:1 r:2][d a:2 r:2][d a:3 r:2][][][][]
# data bitmap   11110000
# data         [(.,0) (..,0) (u,1) (z,2) (s,3)][(.,1) (..,0)][(.,2) (..,0)][(.,3) (..,0)][][][][]
#
# creat("/z/x");
#
# inode bitmap  11111000
# inodes       [d a:0 r:5][d a:1 r:2][d a:2 r:2][d a:3 r:2][f a:-1 r:1][][][]
# data bitmap   11110000
# data         [(.,0) (..,0) (u,1) (z,2) (s,3)][(.,1) (..,0)][(.,2) (..,0) (x,4)][(.,3) (..,0)][][][][]
#
# link("/z/x", "/u/b");
#
# inode bitmap  11111000
# inodes       [d a:0 r:5][d a:1 r:2][d a:2 r:2][d a:3 r:2][f a:-1 r:2][][][]
# data bitmap   11110000
# data         [(.,0) (..,0) (u,1) (z,2) (s,3)][(.,1) (..,0) (b,4)][(.,2) (..,0) (x,4)][(.,3) (..,0)][][][][]
#
# unlink("/u/b");
#
# inode bitmap  11111000
# inodes       [d a:0 r:5][d a:1 r:2][d a:2 r:2][d a:3 r:2][f a:-1 r:1][][][]
# data bitmap   11110000
# data         [(.,0) (..,0) (u,1) (z,2) (s,3)][(.,1) (..,0)][(.,2) (..,0) (x,4)][(.,3) (..,0)][][][][]
#
# fd=open("/z/x", O_WRONLY|O_APPEND); write(fd, buffer, BLOCKSIZE); close(fd);
#
# inode bitmap  11111000
# inodes       [d a:0 r:5][d a:1 r:2][d a:2 r:2][d a:3 r:2][f a:4 r:1][][][]
# data bitmap   11111000
# data         [(.,0) (..,0) (u,1) (z,2) (s,3)][(.,1) (..,0)][(.,2) (..,0) (x,4)][(.,3) (..,0)][v][][][]
#
# creat("/u/b");
#
# inode bitmap  11111100
# inodes       [d a:0 r:5][d a:1 r:2][d a:2 r:2][d a:3 r:2][f a:4 r:1][f a:-1 r:1][][]
# data bitmap   11111000
# data         [(.,0) (..,0) (u,1) (z,2) (s,3)][(.,1) (..,0) (b,5)][(.,2) (..,0) (x,4)][(.,3) (..,0)][v][][][]
```

Q2. Now do the same, using different random seeds (say 21, 22, 23, 24), except run with the `-r` flag, thus making you guess the state change while being shown the operation. What can you conclude about the inode and data-block allocation algorithms, in terms of which blocks they prefer to allocate?

```sh
$ ./vsfs.py -s 21 -r
# Initial state
#
# inode bitmap  10000000
# inodes       [d a:0 r:2][][][][][][][]
# data bitmap   10000000
# data         [(.,0) (..,0)][][][][][][][]
#
# mkdir("/o");
#
# inode bitmap  11000000
# inodes       [d a:0 r:3][d a:1 r:2][][][][][][]
# data bitmap   11000000
# data         [(.,0) (..,0), (o,1)][(.,1) (..,0)][][][][][][]
#
# creat("/b");
#
# inode bitmap  11100000
# inodes       [d a:0 r:3][d a:1 r:2][f a:-1 r:1][][][][][]
# data bitmap   11000000
# data         [(.,0) (..,0), (o,1), (b,2)][(.,1) (..,0)][][][][][][]
#
# creat("/o/q");
#
# inode bitmap  11110000
# inodes       [d a:0 r:3][d a:1 r:2][f a:-1 r:1][f a:-1 r:1][][][][]
# data bitmap   11000000
# data         [(.,0) (..,0), (o,1), (b,2)][(.,1) (..,0), (q,3)][][][][][][]
#
# fd=open("/b", O_WRONLY|O_APPEND); write(fd, buf, BLOCKSIZE); close(fd);
#
# inode bitmap  11110000
# inodes       [d a:0 r:3][d a:1 r:2][f a:2 r:1][f a:-1 r:1][][][][]
# data bitmap   11100000
# data         [(.,0) (..,0), (o,1), (b,2)][(.,1) (..,0), (q,3)][b0][][][][][]
#
# fd=open("/o/q", O_WRONLY|O_APPEND); write(fd, buf, BLOCKSIZE); close(fd);
#
# inode bitmap  11110000
# inodes       [d a:0 r:3][d a:1 r:2][f a:2 r:1][f a:3 r:1][][][][]
# data bitmap   11110000
# data         [(.,0) (..,0), (o,1), (b,2)][(.,1) (..,0), (q,3)][b0][q0][][][][]
#
# creat("/o/j");
#
# inode bitmap  11111000
# inodes       [d a:0 r:3][d a:1 r:2][f a:2 r:1][f a:3 r:1][f a:-1 r:1][][][]
# data bitmap   11110000
# data         [(.,0) (..,0), (o,1), (b,2)][(.,1) (..,0), (q,3), (j,4)]
#              [b0][q0][][][][]
#
# unlink("/b");
#
# inode bitmap  11011000
# inodes       [d a:0 r:3][d a:1 r:2][][f a:3 r:1][f a:-1 r:1][][][]
# data bitmap   11010000
# data         [(.,0) (..,0), (o,1)][(.,1) (..,0), (q,3), (j,4)][][q0][][][][]
#
# fd=open("/o/j", O_WRONLY|O_APPEND); write(fd, buf, BLOCKSIZE); close(fd);
#
# inode bitmap  11011000
# inodes       [d a:0 r:3][d a:1 r:2][][f a:3 r:1][f a:2 r:1][][][]
# data bitmap   11110000
# data         [(.,0) (..,0), (o,1)][(.,1) (..,0), (q,3), (j,4)][j0][q0][][][][]
#
# creat("/o/x");
#
# inode bitmap  11111000
# inodes       [d a:0 r:3][d a:1 r:2][f a:-1 r:1][f a:3 r:1][f a:2 r:1][][][]
# data bitmap   11110000
# data         [(.,0) (..,0), (o,1)]
#              [(.,1) (..,0), (q,3), (j,4), (x,2)][j0][q0][][][][]
#
# mkdir("/o/t");
#
# inode bitmap  11111100
# inodes       [d a:0 r:3][d a:1 r:3][f a:-1 r:1][f a:3 r:1][f a:2 r:1]
#              [d a:4 r:2][][]
# data bitmap   11111000
# data         [(.,0) (..,0), (o,1)]
#              [(.,1) (..,0), (q,3), (j,4), (x,2), (t,5)][j0][q0]
#              [(.,5) (..,1)][][][]
```

The first available block found in the bitmap is allocated.

Q3. Now reduce the number of data blocks in the file system, to very low numbers (say two), and run the simulator for a hundred or so requests. What types of files end up in the file system in this highly-constrained layout? What types of operations would fail?

Q4. Now do the same, but with inodes. With very few inodes, what types of operations can succeed? Which will usually fail? What is the final state of the file system likely to be?
