#ifndef CPUINFO_HPP
#define CPUINFO_HPP

#include <cstdint>
#include <string>

struct CPUInfo {
    // Vendor & Model
    std::string vendor;
    std::string model;
    
    // Cores & Threads
    uint32_t physicalCores = 0;
    uint32_t logicalCores = 0;
    
    // Cache sizes (in KB)
    uint32_t l1CacheSize = 0;
    uint32_t l2CacheSize = 0;
    uint32_t l3CacheSize = 0;
    
    // SIMD Support
    bool sse = false;
    bool sse2 = false;
    bool sse3 = false;
    bool ssse3 = false;
    bool sse41 = false;
    bool sse42 = false;
    bool avx = false;
    bool avx2 = false;
    bool avx512 = false;
    
    // Other features
    bool hyperthreading = false;
    bool aes = false;
    bool fma = false;
    bool f16c = false;
    bool popcnt = false;
    bool bmi1 = false;
    bool bmi2 = false;

    static CPUInfo Detect();
    
    // helpers
    std::string GetFeatureString() const;
    bool IsIntel() const;
    bool IsAMD() const;
    void Print() const;
};

#endif