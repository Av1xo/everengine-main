#ifndef MEMORYINFO_HPP
#define MEMORYINFO_HPP

#include <cstdint>
struct MemoryInfo {
    uint64_t totalRAM_MB = 0;
    uint64_t freeRAM_MB = 0;
    uint64_t availableRAM_MB = 0;

    uint64_t totalSwap_MB = 0;
    uint64_t freeSwap_MB = 0;

    static MemoryInfo Detect();
    void Print() const;
};

#endif // !MEMORYINFO_HPP