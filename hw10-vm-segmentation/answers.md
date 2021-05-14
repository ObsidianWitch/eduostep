1. First let’s use a tiny address space to translate some addresses. Here’s a simple set of parameters with a few different random seeds; can you translate the addresses?

    ```sh
    $ ./segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 0
    ARG seed 0
    ARG address space size 128 (= 2**7 - 1; 7 bits 0->6; 0->127)
    ARG phys mem size 512

    Segment register information:

      Segment 0 base  (grows positive) : 0x00000000 (decimal 0)
      Segment 0 limit                  : 20

      Segment 1 base  (grows negative) : 0x00000200 (decimal 512)
      Segment 1 limit                  : 20

    Virtual Address Trace
      VA  0: 0x0000006c = 108 = 0b1101100
        * segment: SEG1
        * bounds check: offset = address_space_size - virtual_address
                        offset <= segment_limit
                        128 - 108 = 20 <= 20
        * physical address: segment_base - offset = 512 - 20 = 492
      VA  1: 0x00000061 = 97  = 0b1100001 --> SEG1; 31 > 20; segfault
      VA  2: 0x00000035 = 53  = 0b0110101 --> SEG0; 53 > 20; segfault
      VA  3: 0x00000021 = 33  = 0b0100001 --> SEG0; 33 > 20; segfault
      VA  4: 0x00000041 = 65  = 0b1000001 --> SEG1; 63 > 20; segfault
    ```

    ```sh
    $ ./segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 1
    Virtual Address Trace
      VA  0: 0x00000011 =  17 = 0b0010001 --> SEG0; 17 <= 20; 0 + 17 = 17
      VA  1: 0x0000006c = 108 = 0b1101100 --> SEG1; 20 <= 20; 512 - 20 = 492
      VA  2: 0x00000061 =  97 = 0b1100001 --> SEG1; 31 > 20; segfault
      VA  3: 0x00000020 =  32 = 0b0100000 --> SEG0; 32 > 20; segfault
      VA  4: 0x0000003f =  63 = 0b0111111 --> SEG0; 63 > 20; segfault
    ```

    ```sh
    $ ./segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 2
    Virtual Address Trace
      VA  0: 0x0000007a = 122 = 0b1111010 --> SEG1; 6  <= 20; 506
      VA  1: 0x00000079 = 121 = 0b1111001 --> SEG1; 7  <= 20; 505
      VA  2: 0x00000007 =   7 = 0b0000111 --> SEG0; 7  <= 20; 7
      VA  3: 0x0000000a =  10 = 0b0001010 --> SEG0; 10 <= 20; 10
      VA  4: 0x0000006a = 106 = 0b1101010 --> SEG1; 22 >  20; segfault
    ```

2. Now, let’s see if we understand this tiny address space we’ve constructed (using the parameters from the question above). What is the highest legal virtual address in segment 0? What about the lowest legal virtual address in segment 1? What are the lowest and highest illegal addresses in this entire address space? Finally, how would you run segmentation.py with the -A flag to test if you are right?

    * highest legal virtual address in segment 0:
      seg0.limit - 1 = 19
    * lowest legal virtual address in segment 1:
      address_space_size - seg1.limit = 108
    * lowest illegal address in this virtual address space: 20
    * highest illegal address in this virtual address space: 107

    ```sh
    $ ./segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 \
                        -A "$(seq -s',' 0 127)" -c
    Virtual Address Trace
      VA  0: 0x00000000 = 0 --> VALID in SEG0: 0x00000000 = 0
      ...
      VA 19: 0x00000013 = 19 --> VALID in SEG0: 0x00000013 = 19
      VA 20: 0x00000014 = 20 --> SEGMENTATION VIOLATION (SEG0)
      ...
      VA 107: 0x0000006b = 107 --> SEGMENTATION VIOLATION (SEG1)
      VA 108: 0x0000006c = 108 --> VALID in SEG1: 0x000001ec = 492
      ...
      VA 127: 0x0000007f = 127 --> VALID in SEG1: 0x000001ff = 511
    ```

3. Let’s say we have a tiny 16-byte address space in a 128-byte physical memory. What base and bounds would you set up so as to get the simulator to generate the following translation results for the specified address stream: valid, valid, violation, ..., violation, valid, valid? Assume the following parameters: `segmentation.py -a 16 -p 128 -A 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15`

    * `l0 = l1 = 2`
    * b0 and b1 should be set so that the segments fit in physical memory and that the segments don't overlap
    * example: `./segmentation.py -a 16 -p 128 -A 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 --b0=18 --l0=2 --b1=102 --l1=2 -c`

4. Assume we want to generate a problem where roughly 90% of the randomly-generated virtual addresses are valid (not segmentation violations). How should you configure the simulator to do so? Which parameters are important to getting this outcome?

    `l0 + l1 = 0.9 * vaddr_space_size`

5. Can you run the simulator such that no virtual addresses are valid? How?

    `l0 = l1 = 0`
