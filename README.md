# CacheAnalyzer
CacheAnalyzer is a trace-driven CPU cache simulator written in modern C++. It allows computer science students and developers to study and analyze the behavior and performance impact of various cache designs and data access patterns.

## Specifications
This section details the fixed and flexible design constraints of the simulator.
* **Associativity**: Flexible (1 way - 1024 ways).
* **Cache line size**: 64 bytes.
* **Total cache size**: Flexible (8KB - 64KB).
* **Replacement policy**: Full LRU / Random Replacement.
* **Write policy**: Write-Through.
* **Allocation policy**: No-Write-Allocate.
* **Supported instructions**: Read / Write.
* **Instruction data size**: 4 bytes.
* **Data generation**: Random data.
* **Memory layout**: 32-bit addresses.
* **Address alignment**: 4 bytes.
* **RAM size**: 4096KB.
* **Output**: Standard output (CLI).
* **Output detail level**: Flexible (Minimal / Extended).
