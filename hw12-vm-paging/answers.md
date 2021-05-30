1. Before doing any translations, let’s use the simulator to study how linear page tables change size given different parameters. Compute the size of linear page tables as different parameters change. Some suggested inputs are below; by using the -v flag, you can see how many page-table entries are filled. First, to understand how linear page table size changes as the address space grows, run with these flags: [...] Then, to understand how linear page table size changes as page size grows: [...] Before running any of these, try to think about the expected trends. How should page-table size change as the address space grows? As the page size grows? Why not use big pages in general?

    ```
    # -P 1k -a 1m -p 512m -v -n 0
    1 Mi / 1 Ki = 1 Ki = 1024 page-table entries
    # -P 1k -a 2m -p 512m -v -n 0
    2 Mi / 1 Ki = 2048 page-table entries
    # -P 1k -a 4m -p 512m -v -n 0
    4 Mi / 1 Ki = 4096 page-table entries
    ```

    ```
    # -P 1k -a 1m -p 512m -v -n 0
    1 Mi / 1 Ki = 1 Ki = 1024 page-table entries
    # -P 2k -a 1m -p 512m -v -n 0
    1 Mi / 2 Ki = 512 page-table entries
    # -P 4k -a 1m -p 512m -v -n 0
    1 Mi / 4 Ki = 256 page-table entries
    ```

    As the address space grows, more pages-table entries are needed to cover the whole address space. As page size grows, less pages are needed to cover the address space and thus less page-table entries are needed. `page-table size = address space size / page size`. Big pages lead to internal fragmentation.

2. Now let’s do some translations. Start with some small examples, and change the number of pages that are allocated to the address space with the -u flag. What happens as you increase the percentage of pages that are allocated in each address space?

    ```
    # -P 1k -a 16k -p 32k -v -u 0
    ARG address space size 16k = 2^14: 14 bits virtual address space
    ARG phys mem size 32k = 2^15: 15 bits physical address space
    ARG page size 1k
    16 Ki / 1 Ki = 16 = 2^4: 4 bits VPN
    32 Ki / 1 Ki = 32 = 2^5: 5 bits FPN

    Page Table (from entry 0 down to the max size)
      [       0]  0x00000000
      [       1]  0x00000000
      [       2]  0x00000000
      [       3]  0x00000000
      [       4]  0x00000000
      [       5]  0x00000000
      [       6]  0x00000000
      [       7]  0x00000000
      [       8]  0x00000000
      [       9]  0x00000000
      [      10]  0x00000000
      [      11]  0x00000000
      [      12]  0x00000000
      [      13]  0x00000000
      [      14]  0x00000000
      [      15]  0x00000000

    Virtual Address Trace
      VA 0x00003a39 --> invalid (VPN=0b1110=14)
      VA 0x00003ee5 --> invalid (VPN=0b1111=15)
      VA 0x000033da --> invalid (VPN=0b1100=12)
      VA 0x000039bd --> invalid (VPN=0b1110=14)
      VA 0x000013d9 --> invalid (VPN=0b0100=4)
    ```
    ```
    # -P 1k -a 16k -p 32k -v -u 25
    Page Table (from entry 0 down to the max size)
      [       0]  0x80000018
      [       1]  0x00000000
      [       2]  0x00000000
      [       3]  0x00000000
      [       4]  0x00000000
      [       5]  0x80000009
      [       6]  0x00000000
      [       7]  0x00000000
      [       8]  0x80000010
      [       9]  0x00000000
      [      10]  0x80000013
      [      11]  0x00000000
      [      12]  0x8000001f
      [      13]  0x8000001c
      [      14]  0x00000000
      [      15]  0x00000000

    Virtual Address Trace
      VA 0x00003986 --> VPN=0b1110=14 invalid
      VA 0x00002bc6 --> VPN=0b1010=10 offset=0b1111000110=966
                        PFN=0x13=0b10011 PA=0x4fc6=20422
      VA 0x00001e37 --> VPN=7 invalid
      VA 0x00000671 --> VPN=1 invalid
      VA 0x00001bc9 --> VPN=6 invalid
    ```
    ```
    # -P 1k -a 16k -p 32k -v -u 50
    Page Table (from entry 0 down to the max size)
      [       0]  0x80000018
      [       1]  0x00000000
      [       2]  0x00000000
      [       3]  0x8000000c
      [       4]  0x80000009
      [       5]  0x00000000
      [       6]  0x8000001d
      [       7]  0x80000013
      [       8]  0x00000000
      [       9]  0x8000001f
      [      10]  0x8000001c
      [      11]  0x00000000
      [      12]  0x8000000f
      [      13]  0x00000000
      [      14]  0x00000000
      [      15]  0x80000008

    Virtual Address Trace
      VA 0x00003385 --> VPN=12 offset=0b1110000101=901
                        FPN=0xf=0b01111 PA=0x3f85
      VA 0x0000231d --> VPN=8 invalid
      VA 0x000000e6 --> VPN=0 offset=0b11100110
                        FPN=0b11000 PA=0x60e6
      VA 0x00002e0f --> VPN=11 invalid
      VA 0x00001986 --> VPN=6 PA=0x7586
    ```

    Increasing the percentage of allocated pages increases the number of valid generated virtual addresses.

3. Now let’s try some different random seeds, and some different (and sometimes quite crazy) address-space parameters, for variety. Which of these parameter combinations are unrealistic? Why?

    ```
    # -P 8 -a 32 -p 1024 -v -s 1
    # page size too small -> big page-table

    # -P 8k -a 32k  -p 1m   -v -s 2

    # -P 1m -a 256m -p 512m -v -s 3
    # page size too big -> internal fragmentation
    ```

4. Use the program to try out some other problems. Can you find the limits of where the program doesn’t work anymore? For example, what happens if the address-space size is bigger than physical memory?

    ```
    $ ./paging-linear-translate.py -a 64k
    Error: physical memory size must be GREATER than address space size (for this simulation)

    $ ./paging-linear-translate.py -P 32k # page size > address space
    $ ./paging-linear-translate.py -P 35 # address space not divisible by page size
    Error in argument: address space must be a multiple of the pagesize
    ```
