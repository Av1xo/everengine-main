#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>

struct VolumeInfo {
    std::string device;       // e.g. /dev/sda1 or \\?\Volume{...} or UUID
    std::string mountPoint;   // e.g. /, /home, C:
    std::string fsType;       // e.g. ext4, vfat, ntfs, apfs
    bool removable = false;
    bool readOnly = false;

    uint64_t totalBytes = 0;
    uint64_t freeBytes = 0;      // free for all users
    uint64_t availableBytes = 0; // free for calling user

    uint64_t totalMB() const { return totalBytes / (1024ull*1024ull); }
    uint64_t freeMB()  const { return freeBytes / (1024ull*1024ull); }
    uint64_t availMB() const { return availableBytes / (1024ull*1024ull); }
};

struct StorageInfo {
    std::vector<VolumeInfo> volumes;

    static StorageInfo Detect(); // enumerates volumes
    void Print() const;
};


#endif // !STORAGE_HPP