# fast-histogram
Fast byte histogram

## Benchmark results

```
Intel(R) Core(TM) i7-8565U CPU
---------------------------------------------------------------------------
Benchmark                                 Time             CPU   Iterations
---------------------------------------------------------------------------
BenchByteHistogram/8                   12.0 ns         11.9 ns     58274638
BenchByteHistogram/64                  30.2 ns         30.0 ns     23442574
BenchByteHistogram/512                  216 ns          214 ns      3251773
BenchByteHistogram/4096                1698 ns         1695 ns       405521
BenchByteHistogram/32768              13423 ns        13395 ns        51563
BenchByteHistogram/262144            109462 ns       109213 ns         6323
BenchByteHistogram/1048576           434452 ns       433465 ns         1597
BenchByteHistogramX4/8                 84.6 ns         84.4 ns      8047103
BenchByteHistogramX4/64                 109 ns          109 ns      6384892
BenchByteHistogramX4/512                268 ns          268 ns      2599787
BenchByteHistogramX4/4096              1577 ns         1573 ns       445280
BenchByteHistogramX4/32768            11655 ns        11630 ns        60094
BenchByteHistogramX4/262144           91745 ns        91548 ns         7562
BenchByteHistogramX4/1048576         368987 ns       368231 ns         1893
BenchByteHistogramLong16/8             9045 ns         8993 ns        76120
BenchByteHistogramLong16/64            9040 ns         8978 ns        76082
BenchByteHistogramLong16/512           9214 ns         9149 ns        72338
BenchByteHistogramLong16/4096          9936 ns         9870 ns        70554
BenchByteHistogramLong16/32768        15562 ns        15528 ns        44572
BenchByteHistogramLong16/262144       62639 ns        62503 ns        10856
BenchByteHistogramLong16/1048576     220407 ns       219907 ns         3165
```

