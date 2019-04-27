# Test Random
Tesing some pseudo random number generators for my own purpose.  
For my purpose, pseudo random number generators should be fast, uniformly distributed, but need not be secure.

# Tesing
[The Dieharder (https://webhome.phy.duke.edu/~rgb/General/dieharder.php)](https://webhome.phy.duke.edu/~rgb/General/dieharder.php) is adopted to test generators.  
I generate 2 GiB binary files for testing each generators.

# Random Number Generators
Generators included are below.

|Name|Bits|Cycle Length|
|:---|---:|---:|
|Xoshiro128Plus|32|2^128|
|WELL512|32|2^512|

# Results
In the following table, list up items which are **not passed**.

|Name|Test Name|ntup|tsamples|psamples|p-value|Assessment|
|:---|---:|---:|---:|---:|---:|---:|
|Xoshiro128Plus| | | | | | |
||diehard_runs|   0|    100000|     100|0.99941604|   WEAK|
||sts_serial|  12|    100000|     100|0.99663722|   WEAK|
||rgb_lagged_sum|  23|   1000000|     100|0.99821351|   WEAK|
|WELL512| | | | | | |
||diehard_parking_lot|   0|     12000|     100|0.99970614|   WEAK|
||diehard_craps|   0|    200000|     100|0.00048876|   WEAK|
||sts_serial|   7|    100000|     100|0.00124432|   WEAK|
||sts_serial|  11|    100000|     100|0.99718446|   WEAK|

# License
This is free and unencumbered software released into the public domain. See a text file 'LICENSE'.

# References
[xoshiro|xoroshiro (http://xoshiro.di.unimi.it/)](http://xoshiro.di.unimi.it/)
