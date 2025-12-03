#include "MemoryInfo.hpp"
#include <iostream>

#ifdef __linux__
#include <fstream>
#include <string>
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef __APPLE__
#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/mach.h>
#endif

MemoryInfo MemoryInfo::Detect() {
    MemoryInfo info;

    // -------------------------
    // LINUX
    // -------------------------
#ifdef __linux__
    std::ifstream mem("/proc/meminfo");
    std::string key;
    uint64_t value;
    std::string unit;

    while (mem >> key >> value >> unit) {
        if (key == "MemTotal:") info.totalRAM_MB = value / 1024;
        else if (key == "MemFree:") info.freeRAM_MB = value / 1024;
        else if (key == "MemAvailable:") info.availableRAM_MB = value / 1024;

        else if (key == "SwapTotal:") info.totalSwap_MB = value / 1024;
        else if (key == "SwapFree:") info.freeSwap_MB = value / 1024;
    }
#endif

    // -------------------------
    // WINDOWS
    // -------------------------
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(memInfo);

    if (GlobalMemoryStatusEx(&memInfo)) {
        info.totalRAM_MB = memInfo.ullTotalPhys / (1024ull * 1024ull);
        info.availableRAM_MB = memInfo.ullAvailPhys / (1024ull * 1024ull);
        info.freeRAM_MB = info.availableRAM_MB;

        info.totalSwap_MB = memInfo.ullTotalPageFile / (1024ull * 1024ull);
        info.freeSwap_MB = memInfo.ullAvailPageFile / (1024ull * 1024ull);
    }
#endif

    // -------------------------
    // macOS (Darwin)
    // -------------------------
#ifdef __APPLE__
    int64_t totalMem = 0;
    size_t len = sizeof(totalMem);
    sysctlbyname("hw.memsize", &totalMem, &len, NULL, 0);
    info.totalRAM_MB = totalMem / (1024ull * 1024ull);

    mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
    vm_statistics64_data_t vm;
    host_statistics64(mach_host_self(), HOST_VM_INFO, (host_info64_t)&vm, &count);

    uint64_t pageSize;
    len = sizeof(pageSize);
    sysctlbyname("hw.pagesize", &pageSize, &len, NULL, 0);

    uint64_t freeBytes =
        (uint64_t)vm.free_count * pageSize +
        (uint64_t)vm.inactive_count * pageSize;

    info.freeRAM_MB = freeBytes / (1024ull * 1024ull);
    info.availableRAM_MB = info.freeRAM_MB;

    info.totalSwap_MB = 0;
    info.freeSwap_MB = 0;
#endif

    return info;
}

void MemoryInfo::Print() const {
    std::cout << "=== Memory Information ===\n";
    std::cout << "Total RAM:      " << totalRAM_MB << " MB\n";
    std::cout << "Free RAM:       " << freeRAM_MB << " MB\n";
    std::cout << "Available RAM:  " << availableRAM_MB << " MB\n";
    std::cout << "Swap Total:     " << totalSwap_MB << " MB\n";
    std::cout << "Swap Free:      " << freeSwap_MB << " MB\n";
    std::cout << "==========================\n";
}
