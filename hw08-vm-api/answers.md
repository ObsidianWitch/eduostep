1. First, write a simple program called null.c that creates a pointer to an integer, sets it to NULL, and then tries to dereference it. Compile this into an executable called null. What happens when you run this program?

    A segmentation fault (`Segmentation fault (core dumped)`) occurs here because we dereference an address that doesn't point to allocated memory and try to write to it. Dereferencing a NULL pointer has undefined behavior, meaning that depending on the platform it doesn't always guarantee a crash and might cause subtle and hard to notice problems by accessing memory it isn't supposed to access.

    **Note**: The core is dumped in the file or to the command specified by `/proc/sys/kernel/core_pattern`  (https://stackoverflow.com/a/2067406). On my system it contains `|/usr/lib/systemd/systemd-coredump %P %u %g %s %t %c %h`. And running `coredumpctl info` shows info about the last core dump.

2. Next, compile this program with symbol information included (with the -g flag). Doing so let’s put more information into the executable, enabling the debugger to access more useful information about variable names and the like. Run the program under the debugger by typing gdb null and then, once gdb is running, typing run. What does gdb show you?

    ```
    (gdb) run
    Starting program: [...]/out/bad-null

    Program received signal SIGSEGV, Segmentation fault.
    0x0000555555555130 in main (argc=1, argv=0x7fffffffe788) at bad-null.c:5
    5	   *x = 10;
    (gdb)
    ```

3. Finally, use the valgrind tool on this program. We’ll use the memcheck tool that is a part of valgrind to analyze what happens. Run this by typing in the following: valgrind --leak-check=yes null. What happens when you run this? Can you interpret the output from the tool?

    ```
    ==2431141== Command: out/bad-null
    ==2431141==
    ==2431141== Invalid write of size 4
    ==2431141==    at 0x109130: main (bad-null.c:5)
    ==2431141==  Address 0x0 is not stack'd, malloc'd or (recently) free'd
    ==2431141==
    ==2431141==
    ==2431141== Process terminating with default action of signal 11 (SIGSEGV): dumping core
    ==2431141==  Access not within mapped region at address 0x0
    ==2431141==    at 0x109130: main (bad-null.c:5)
    ==2431141==  If you believe this happened as a result of a stack
    ==2431141==  overflow in your program's main thread (unlikely but
    ==2431141==  possible), you can try to increase the size of the
    ==2431141==  main thread stack using the --main-stacksize= flag.
    ==2431141==  The main thread stack size used in this run was 8388608.
    ==2431141==
    ==2431141== HEAP SUMMARY:
    ==2431141==     in use at exit: 0 bytes in 0 blocks
    ==2431141==   total heap usage: 0 allocs, 0 frees, 0 bytes allocated
    ==2431141==
    ==2431141== All heap blocks were freed -- no leaks are possible
    ==2431141==
    ==2431141== For lists of detected and suppressed errors, rerun with: -s
    ==2431141== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
    Segmentation fault (core dumped)
    ```

    The process tries to write to address 0x0 (null pointer) which hasn't been mapped to a physical address in memory. For this reason, the OS terminated the process with signal SIGSEGV (segmentation violation).

4. Write a simple program that allocates memory using malloc() but forgets to free it before exiting. What happens when this program runs? Can you use gdb to find any problems with it? How about valgrind (again with the --leak-check=yes flag)?

    The program executes without crashing and gdb doesn't catch any error. Valgrind on the other hand reports a memory leak.

    ```
    ==2472296== 4 bytes in 1 blocks are definitely lost in loss record 1 of 1
    ==2472296==    at 0x483E77F: malloc (vg_replace_malloc.c:307)
    ==2472296==    by 0x109151: main (bad-leak.c:4)
    ```

5. Write a program that creates an array of integers called data of size 100 using malloc; then, set data[100] to zero. What happens when you run this program? What happens when you run this program using valgrind? Is the program correct?

    The program executes without crashing but is incorrect since our array is defined for i in [0;99]. Accessing an array out of bounds is undefined behavior. Valgrind reports an invalid write.

    ```
    ==2476724== Invalid write of size 4
    ==2476724==    at 0x109170: main (bad-index.c:5)
    ==2476724==  Address 0x4a3b1d0 is 0 bytes after a block of size 400 alloc'd
    ==2476724==    at 0x483E77F: malloc (vg_replace_malloc.c:307)
    ==2476724==    by 0x109161: main (bad-index.c:4)
    ```

6. Create a program that allocates an array of integers (as above), frees them, and then tries to print the value of one of the elements of the array. Does the program run? What happens when you use valgrind on it?

    It prints a different value from the one specified before the `free()` and the program doesn't crash. Valgrind reports an invalid read.

    ```
    ==2479242== Invalid read of size 4
    ==2479242==    at 0x109190: main (bad-free-1.c:8)
    ==2479242==  Address 0x4a3b040 is 0 bytes inside a block of size 400 free'd
    ==2479242==    at 0x483F9AB: free (vg_replace_malloc.c:538)
    ==2479242==    by 0x10918B: main (bad-free-1.c:7)
    ==2479242==  Block was alloc'd at
    ==2479242==    at 0x483E77F: malloc (vg_replace_malloc.c:307)
    ==2479242==    by 0x109171: main (bad-free-1.c:5)
    ```

7. Now pass a funny value to free (e.g., a pointer in the middle of the array you allocated above). What happens? Do you need tools to find this type of problem?

    ```
    $ out/bad-free-2
    free(): invalid pointer
    Aborted (core dumped)
    ```

8. Try out some of the other interfaces to memory allocation. For example, create a simple vector-like data structure and related routines that use realloc() to manage the vector. Use an array to store the vectors elements; when a user adds an entry to the vector, use realloc() to allocate more space for it. How well does such a vector perform? How does it compare to a linked list? Use valgrind to help you find bugs.

    * See `vector.c` (only insert/delete at end were implemented).
    * https://en.wikipedia.org/wiki/Array_data_structure#Comparison_with_other_data_structures

9. Spend more time and read about using gdb and valgrind. Knowing your tools is critical; spend the time and learn how to become an expert debugger in the UNIX and C environment.
