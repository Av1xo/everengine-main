#if defined(_MSC_VER)
#   include <intrin.h>
#   include <windows.h>
#elif defined(__GNUC__)
#   include <cpuid.h>
#   include <unistd.h>
#endif

#include <cstring>
#include <sstream>
#include <iostream>
#include "CPUinfo.hpp"

// Platform-independent CPUID wrapper
static void cpuid(int out[4], int function_id, int subfunction_id = 0) {
#if defined(_MSC_VER)
    __cpuidex(out, function_id, subfunction_id);
#elif defined(__GNUC__)
    __cpuid_count(function_id, subfunction_id, out[0], out[1], out[2], out[3]);
#else
    out[0] = out[1] = out[2] = out[3] = 0;
#endif
}

CPUInfo CPUInfo::Detect() {
    CPUInfo info;
    int data[4];
    
    // === Vendor String ===
    cpuid(data, 0, 0);
    int maxFunctionId = data[0];
    
    char vendorStr[13];
    memcpy(vendorStr + 0, &data[1], 4);  // EBX
    memcpy(vendorStr + 4, &data[3], 4);  // EDX
    memcpy(vendorStr + 8, &data[2], 4);  // ECX
    vendorStr[12] = '\0';
    info.vendor = vendorStr;
    
    // === Brand String ===
    cpuid(data, 0x80000000, 0);
    int maxExtendedId = data[0];
    
    if (maxExtendedId >= 0x80000004) {
        char brandStr[49] = {0};
        cpuid((int*)(brandStr +  0), 0x80000002, 0);
        cpuid((int*)(brandStr + 16), 0x80000003, 0);
        cpuid((int*)(brandStr + 32), 0x80000004, 0);
        
        const char* trimmed = brandStr;
        while (*trimmed == ' ') trimmed++;
        info.model = trimmed;
    }
    
    // === Core Count ===
#if defined(_WIN32)
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    info.logicalCores = sysinfo.dwNumberOfProcessors;

    DWORD length = 0;
    GetLogicalProcessorInformation(nullptr, &length);
    if (length > 0) {
        auto buffer = new SYSTEM_LOGICAL_PROCESSOR_INFORMATION[length / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION)];
        if (GetLogicalProcessorInformation(buffer, &length)) {
            DWORD physicalCoreCount = 0;
            for (DWORD i = 0; i < length / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); i++) {
                if (buffer[i].Relationship == RelationProcessorCore) {
                    physicalCoreCount++;
                }
            }
            info.physicalCores = physicalCoreCount;
        }
        delete[] buffer;
    }
#elif defined(__linux__) || defined(__APPLE__)
    info.logicalCores = sysconf(_SC_NPROCESSORS_ONLN);
    info.physicalCores = info.logicalCores; // Approximation
#endif
    
    if (info.physicalCores == 0) {
        info.physicalCores = info.logicalCores;
    }
    
    info.hyperthreading = (info.logicalCores > info.physicalCores);
    
    if (maxFunctionId >= 1) {
        cpuid(data, 1, 0);
        
        // ECX features
        info.sse3   = (data[2] & (1 << 0))  != 0;
        info.ssse3  = (data[2] & (1 << 9))  != 0;
        info.fma    = (data[2] & (1 << 12)) != 0;
        info.sse41  = (data[2] & (1 << 19)) != 0;
        info.sse42  = (data[2] & (1 << 20)) != 0;
        info.popcnt = (data[2] & (1 << 23)) != 0;
        info.aes    = (data[2] & (1 << 25)) != 0;
        info.avx    = (data[2] & (1 << 28)) != 0;
        info.f16c   = (data[2] & (1 << 29)) != 0;
        
        // EDX features
        info.sse    = (data[3] & (1 << 25)) != 0;
        info.sse2   = (data[3] & (1 << 26)) != 0;
    }
    
    if (maxFunctionId >= 7) {
        cpuid(data, 7, 0);
        
        // EBX features
        info.avx2   = (data[1] & (1 << 5))  != 0;
        info.bmi1   = (data[1] & (1 << 3))  != 0;
        info.bmi2   = (data[1] & (1 << 8))  != 0;
        info.avx512 = (data[1] & (1 << 16)) != 0;
    }
    
    // === Cache Information ===
    if (maxFunctionId >= 4) {
        for (int i = 0; ; i++) {
            cpuid(data, 4, i);
            int cacheType = data[0] & 0x1F;
            if (cacheType == 0) break;
            
            int cacheLevel = (data[0] >> 5) & 0x7;
            int ways = ((data[1] >> 22) & 0x3FF) + 1;
            int partitions = ((data[1] >> 12) & 0x3FF) + 1;
            int lineSize = (data[1] & 0xFFF) + 1;
            int sets = data[2] + 1;
            
            uint32_t cacheSizeKB = (ways * partitions * lineSize * sets) / 1024;
            
            if (cacheLevel == 1 && (cacheType == 1 || cacheType == 3)) {
                info.l1CacheSize = cacheSizeKB;
            } else if (cacheLevel == 2) {
                info.l2CacheSize = cacheSizeKB;
            } else if (cacheLevel == 3) {
                info.l3CacheSize = cacheSizeKB;
            }
        }
    }
    
    return info;
}

std::string CPUInfo::GetFeatureString() const {
    std::stringstream ss;
    
    if (sse)    ss << "SSE ";
    if (sse2)   ss << "SSE2 ";
    if (sse3)   ss << "SSE3 ";
    if (ssse3)  ss << "SSSE3 ";
    if (sse41)  ss << "SSE4.1 ";
    if (sse42)  ss << "SSE4.2 ";
    if (avx)    ss << "AVX ";
    if (avx2)   ss << "AVX2 ";
    if (avx512) ss << "AVX512 ";
    if (fma)    ss << "FMA ";
    if (aes)    ss << "AES ";
    if (f16c)   ss << "F16C ";
    
    std::string result = ss.str();
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
}

bool CPUInfo::IsIntel() const {
    return vendor.find("Intel") != std::string::npos || 
           vendor == "GenuineIntel";
}

bool CPUInfo::IsAMD() const {
    return vendor.find("AMD") != std::string::npos || 
           vendor == "AuthenticAMD";
}

void CPUInfo::Print() const {
    std::cout << "=== CPU Information ===" << std::endl;
    std::cout << "Vendor: " << vendor << std::endl;
    std::cout << "Model: " << model << std::endl;
    std::cout << "Physical Cores: " << physicalCores << std::endl;
    std::cout << "Logical Cores: " << logicalCores << std::endl;
    std::cout << "Hyperthreading: " << (hyperthreading ? "Yes" : "No") << std::endl;
    
    if (l1CacheSize > 0) std::cout << "L1 Cache: " << l1CacheSize << " KB" << std::endl;
    if (l2CacheSize > 0) std::cout << "L2 Cache: " << l2CacheSize << " KB" << std::endl;
    if (l3CacheSize > 0) std::cout << "L3 Cache: " << l3CacheSize << " KB" << std::endl;
    
    std::cout << "SIMD Features: " << GetFeatureString() << std::endl;
    std::cout << "======================" << std::endl;
}


// int main() {
//     CPUInfo cpu = CPUInfo::Detect();
//     cpu.Print();
//     return 0;
// }
