1. First run with the flags -n 10 -H 0 -p BEST -s 0 to generate a few random allocations and frees. Can you predict what alloc()/free() will return? Can you guess the state of the free list after each request? What do you notice about the free list over time?

    ```sh
    $ ./malloc.py --numOps=10 --headerSize=0 --policy=BEST --seed=0
    seed 0
    size 100 # size of the heap
    baseAddr 1000 # base address of heap
    headerSize 0
    alignment -1 # no alignment
    policy BEST
    listOrder ADDRSORT
    coalesce False
    numOps 10 # number of random ops to generate
    range 10 # max alloc size
    percentAlloc 50 # percent of ops that are allocs
    allocList
    compute False

    ptr[0] = Alloc(3) returned 1000 (searched 1 element)
    List: [ (addr:1003 sz:97) ]

    Free(ptr[0])
    List: [ (addr:1000 sz:3 ), (addr:1003 sz:97) ]

    ptr[1] = Alloc(5) returned 1003 (searched 2 elements)
    List: [ (addr:1000 sz:3 ), (addr:1008 sz:92) ]

    Free(ptr[1])
    List: [ (addr:1000 sz:3 ), (addr:1003 sz:5), (addr:1008 sz:92) ]

    ptr[2] = Alloc(8) returned 1008 (searched 3 elements)
    List: [ (addr:1000 sz:3 ), (addr:1003 sz:5), (addr:1016 sz:84) ]

    Free(ptr[2])
    List: [ (addr:1000 sz:3 ), (addr:1003 sz:5), (addr:1008 sz:8),
            (addr:1016 sz:84) ]

    ptr[3] = Alloc(8) returned 1008 (searched 3 elements)
    List: [ (addr:1000 sz:3 ), (addr:1003 sz:5), (addr:1016 sz:84) ]

    Free(ptr[3])
    List: [ (addr:1000 sz:3 ), (addr:1003 sz:5), (addr:1008 sz:8),
            (addr:1016 sz:84) ]

    ptr[4] = Alloc(2) returned 1000 (searched 4 elements)
    List: [ (addr:1002 sz:1 ), (addr:1003 sz:5), (addr:1008 sz:8),
            (addr:1016 sz:84) ]

    ptr[5] = Alloc(7) returned 1008 (searched 4 elements)
    List: [ (addr:1002 sz:1 ), (addr:1003 sz:5), (addr:1015 sz:1),
            (addr:1016 sz:84) ]
    ```

    The free list grows over time because there is no list coalescing after a free.

2. How are the results different when using a WORST fit policy to search the free list (-p WORST)? What changes?

    Using worst-fit policy seems to lead to more fragmentation since small chunks aren't being reused when possible. Worst-fit policy's time complexity is `O(n)`, the same as best-fit, but since worst-fit leads to more fragmentation it also leads to more elements to search and thus performs worse than best-fit.

3. What about when using FIRST fit (-p FIRST)? What speeds up when you use first fit?

    In the worst case first-fit also has to search the whole list to find a big enough free chunk of memory. But first-fit doesn't always have to search the whole list, because it returns the first usable element. So, in most cases first-fit executes faster than best-fit.

4. For the above questions, how the list is kept ordered can affect the time it takes to find a free location for some of the policies. Use the different free list orderings (-l ADDRSORT, -l SIZESORT+, -l SIZESORT-) to see how the policies and the list orderings interact.

    * SIZESORT+ (ascending): first-fit will return the same result as best-fit while having less element to search.
    * SIZESORT- (descending): first-fit will return the same result as worst-fit while having only 1 element to search.
    * Best-fit and worst-fit act the same (check all elements in the list) regardless of the sorting policies because this implementation doesn't seem to take it into account.
    * Note: Sorting the list has a time-cost and will impact the whole algorithm's complexity. Optimizing the search step's speed by changing the list order won't have any benefit if the time-cost is merely moved to the sorting step.


5. Coalescing of a free list can be quite important. Increase the number of random allocations (say to -n 1000). What happens to larger allocation requests over time? Run with and without coalescing (i.e., without and with the -C flag). What differences in outcome do you see? How big is the free list over time in each case? Does the ordering of the list matter in this case?

    Larger allocation can't be performed over time because the free list is  filled with lots of small chunks. Coalescing solves the problem by merging contiguous free chunks. The free list's length remains small which reduces the number of searches to perform during allocation, it reduces the number of elements to sort in the list, and large allocations can be performed. Coalescing is performed if at least one adjacent neighbor in the free list is contiguous in memory to the current memory chunk being freed. It means ADDRSORT is necessary for it to work correctly.

6. What happens when you change the percent allocated fraction -P to higher than 50? What happens to allocations as it nears 100? What about as the percent nears 0?

    * By increasing the percent of ops that are allocations memory isn't being freed enough/at all and memory can't be allocated anymore since there is no remaining free space in memory.
    * Reducing the percent of allocation ops ensures that memory is freed quicker/as soon as possible.


7. What kind of specific requests can you make to generate a highly-fragmented free space? Use the -A flag to create fragmented free lists, and see how different policies and options change the organization of the free list.

    * Allocations growing in size and then freed without coalescing will lead to fragmentation regardless of the policy. Coalescing is the mechanism to used to avoid fragmentation, and the growing allocations prevent first-fit and best-fit to properly reuse previous chunks. `./malloc.py -c -l ADDRSORT -p BEST -A +1,-0,+2,-1,+3,-2,+4,-3,+5,-4,+6,-5,+7,-6,+8,-7,+9,-8,+10,-9`
    * Coalescing without ADDRSORT will also lead to fragmentation since it only works correctly with ADDRSORT. `./malloc.py -C -c -l SIZESORT- -p BEST -A +1,-0,+2,-1,+3,-2,+4,-3,+5,-4,+6,-5,+7,-6,+8,-7,+9,-8,+10,-9`
    * Allocating a lot of chunks at once and then freeing one out of every two would prevent list coalescing from working regardless of policy and thus lead to fragmentation. `./malloc.py -c -C -l ADDRSORT -p FIRST -A +10,+10,+10,+10,+10,+10,+10,+10,+10,+10,-0,-2,-4,-6,-8`
