1. With a linear page table, you need a single register to locate the page table, assuming that hardware does the lookup upon a TLB miss. How many registers do you need to locate a two-level page table? A three-level table?

    Only one register (Page Directory Base Register, CR3 on x86) is required, the address of the highest/outermost level directory is stored in it. We don't need to store any other address in a register since we store the lower levels in regular pages which are referenced by the successive higher level's entries. The PDBR references the root node (the Page Directory) of a tree-like structure. (See OSTEP ch19, ch20 and https://lwn.net/Articles/253361/).

2. Use the simulator to perform translations given random seeds 0, 1, and 2, and check your answers using the -c flag. How many memory references are needed to perform each lookup?

    ```
    $ ./paging-multilevel-translate.py -s 0
    [...]
    PDBR=108; PD=P[108]
    0x611c=0b11000_01000_11100
    - PDI=0b11000=24; PTI=0b01000=8; offset=11100=28
    - PD[24]=0xa1=0b10100001: valid=1 PFN=0x21=33
    - P[33][8]=0xb5=0b10110101: valid=1 PFN=0x35=53
    - &P[53][28]=0b0110101_11100=0x6bc; P[53][28]=0x08

    0x3da8 = 0b01111_01101_01000
    - PDI=15 PTI=13 offset=18
    - PD[15]=0xd6=0b11010110: valid=1 PFN=86
    - P[86][13]=0x7f=0b01111111: valid=0: invalid page table entry

    0x17f5=0b00101_11111_10101
    - PDI=5 PTI=31 offset=21
    - PD[5]=0xd4=0b11010100: valid=1 PFN=84
    - P[84][31]=0xce=0b11001110: valid=1 PFN=78
    - &P[78][21]=0b1001110_10101=0x9d5; P[78][21]=0x1c

    0x7f6c=0b11111_11011_01100
    - PDI=31 PTI=27 offset=12
    - PD[31]=0xff=0b11111111: valid=1 PFN=127
    - P[255][27]=0x7f=0b01111111: valid=0: invalid page table entry

    0x0bad=0b00010_11101_01101
    - PDI=2 PTI=29 offset=13
    - PD[2]=0xe0=0b11100000: valid=0 PFN=96
    - P[96][29]=0x7f=0b01111111: valid=0: invalid page table entry

    0x6d60=0b11011_01011_00000
    - PDI=27 PTI=11 offset=0
    - PD[27]=0xc2=0b11000010: valid=1 PFN=66
    - P[66][11]=0x7f=0b01111111: valid=0: invalid page table entry

    0x2a5b=0b01010_10010_11011
    - PDI=10 PTI=18 offset=27
    - PD[10]=0xd5=0b11010101: valid=1 PFN=85
    - P[85][18]=0x7f=0b01111111: valid=0: invalid page table entry

    0x4c5e=0b10011_00010_11110
    - PDI=19 PTI=2 offset=30
    - PD[19]=0xf8=0b11111000: valid=1 PFN=120
    - P[120][2]=0x7f=0b01111111: valid=0: invalid page table entry

    0x2592=0b01001_01100_10010
    - PDI=9, PTI=12, offset=18
    - PD[9]=0x9e=0b10011110: valid=1 PFN=30
    - P[30][12]=0xbd=0b10111101: valid=1 PFN=61
    - &P[61][18]=0b0111101_10010=0x7b2, P[61][18]=0x1b

    0x3e99=0b01111_10100_11001
    - PDI=15 PTI=20 offset=25
    - PD[15]=0xd6=0b11010110: valid=1 PFN=86
    - P[86][20]=0xca=0b11001010: valid=1 PFN=74
    - &P[74][25]=0b1001010_11001=0x959; P[74][25]=0x1e
    ```
    ```
    $ ./paging-multilevel-translate.py -s 1
    [...]
    PDBR=17; PD=P[17]
    [...]
    0x390e=0b01110_01000_01110
    - PDI=14 PTI=8 offset=14
    - PD[14]=0x7f=0b01111111: valid=0: invalid page directory entry
    [...]
    ```

    Three memory accesses are needed for a full lookup: get the page-directory entry, get the page-table entry, get the value (see OSTEP figure 20.6).

3. Given your understanding of how cache memory works, how do you think memory references to the page table will behave in the cache? Will they lead to lots of cache hits (and thus fast accesses?) Or lots of misses (and thus slow accesses)?

    The pages constituting the page-table aren't accessed through their virtual address. They are accessed directly through the physical addresses of their entries (`PTEAddr = (PDE.PFN << SHIFT) + (PTIndex * sizeof(PTE))`), and thus there is no translation and no cache involved there.
