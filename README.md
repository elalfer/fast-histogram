# fast-histogram

Fast byte histogram

## Benchmark results

```
Intel(R) Core(TM) i7-8565U CPU
---------------------------------------------------------------------------
Benchmark                                 Time             CPU   Iterations
---------------------------------------------------------------------------
BenchByteHistogram/8                   12.1 ns         12.1 ns     56274842
BenchByteHistogram/64                  36.8 ns         36.7 ns     19317236
BenchByteHistogram/512                  219 ns          219 ns      3168641
BenchByteHistogram/4096                1735 ns         1731 ns       404671
BenchByteHistogram/32768              13923 ns        13857 ns        49346
BenchByteHistogram/262144            130991 ns       129016 ns         6317
BenchByteHistogram/1048576           442646 ns       441199 ns         1146
BenchByteHistogramX4/8                 83.7 ns         83.6 ns      7938830
BenchByteHistogramX4/64                 113 ns          112 ns      6118909
BenchByteHistogramX4/512                273 ns          273 ns      2530391
BenchByteHistogramX4/4096              1594 ns         1590 ns       402642
BenchByteHistogramX4/32768            11514 ns        11495 ns        58348
BenchByteHistogramX4/262144           93960 ns        93782 ns         7095
BenchByteHistogramX4/1048576         373906 ns       373100 ns         1815
BenchByteHistogramX256/8               8627 ns         8521 ns        85375
BenchByteHistogramX256/64              7673 ns         7655 ns        85675
BenchByteHistogramX256/512             7948 ns         7934 ns        83007
BenchByteHistogramX256/4096            9369 ns         9352 ns        70914
BenchByteHistogramX256/32768          20884 ns        20844 ns        32526
BenchByteHistogramX256/262144        114592 ns       114361 ns         5896
BenchByteHistogramX256/1048576       427258 ns       426362 ns         1399
BenchByteHistogramLong16/8             9839 ns         9819 ns        67036
BenchByteHistogramLong16/64            9773 ns         9754 ns        68221
BenchByteHistogramLong16/512           9888 ns         9869 ns        67197
BenchByteHistogramLong16/4096         10743 ns        10712 ns        62444
BenchByteHistogramLong16/32768        16080 ns        16048 ns        42140
BenchByteHistogramLong16/262144       61766 ns        61644 ns        10160
BenchByteHistogramLong16/1048576     218389 ns       217952 ns         3083
```

