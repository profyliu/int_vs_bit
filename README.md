# Integer versus bitwise arithmetics in large-array AND-Count operations

Storing a binary value as a 32-bit integer would waste 31 bits of the memory space. Boolean operations, such as (x and y), would require integer arithmetics or logical operations which could be done more efficiently via bitwise operations if the data is packed in bitblocks. 

This program demonstrates the time difference between integer and bitwise operations for the task of "And Count" on two binary vectors. The "And Count" task for binary vectors x and y is to calculate the element-wise boolean AND for x and y, then count the number of 1s in the result. 

For example: x = [0 1 0 0 1 0 1 1], y = [1 1 0 1 1 0 0 1], the count is 3. 

## Compile
On Windows:
```bash
cl int_vs_bit.c
```

On Linux:
```bash
gcc int_vs_bit.c -o int_vs_bit
```

## Run (example: array size 100 million, run 5 times, verbose = 1)
```bash
# On Windows:
inv_vs_bit 100000000 5 1
# On Linux:
./int_vs_bit 100000000 5 1
```

## Result
Around 90x speedup is consistently observed. 
