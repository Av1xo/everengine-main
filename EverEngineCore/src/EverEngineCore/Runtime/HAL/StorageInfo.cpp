#include "StorageInfo.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>
#include <sstream>
#include <cassert>

#elif defined(__linux__) || defined(__APPLE__)
#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <fstream>
#endif

#ifdef __linux__
#include <mntent.h>    // setmntent, getmntent
#endif

#ifdef __APPLE__
#include <sys/mount.h> // getmntinfo, struct statfs
#endif

// ---------------------------
// Platform implementations
// ---------------------------

StorageInfo StorageInfo::Detect() {
    StorageInfo info;

#ifdef _WIN32
    // Get all logical drives string
    DWORD bufLen = GetLogicalDriveStringsA(0, nullptr);
    if (bufLen == 0) return info;
    std::vector<char> buf(bufLen + 1);
    GetLogicalDriveStringsA(bufLen, buf.data());

    for (char* drive = buf.data(); *drive; drive += strlen(drive) + 1) {
        std::string mount(drive); // e.g. "C:\\"
        UINT type = GetDriveTypeA(mount.c_str());
        // skip CDROM / unknown types if desired - but we'll include most
        VolumeInfo v;
        v.mountPoint = mount;
        v.removable = (type == DRIVE_REMOVABLE);
        v.readOnly = false;

        // volume name, filesystem
        char fsNameBuf[MAX_PATH] = {0};
        char volNameBuf[MAX_PATH] = {0};
        DWORD serial = 0, maxCompLen = 0, flags = 0;
        if (GetVolumeInformationA(mount.c_str(), volNameBuf, MAX_PATH, &serial, &maxCompLen, &flags, fsNameBuf, MAX_PATH)) {
            v.fsType = fsNameBuf;
            // check read-only flag
            v.readOnly = (flags & FILE_READ_ONLY_VOLUME) != 0;
        }

        // disk space
        ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
        if (GetDiskFreeSpaceExA(mount.c_str(), &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes)) {
            v.availableBytes = static_cast<uint64_t>(freeBytesAvailable.QuadPart);
            v.totalBytes = static_cast<uint64_t>(totalNumberOfBytes.QuadPart);
            v.freeBytes = static_cast<uint64_t>(totalNumberOfFreeBytes.QuadPart);
        }

        // device name - construct from drive letter
        v.device = mount;

        info.volumes.push_back(v);
    }
    return info;

#elif defined(__linux__)
    // Use /etc/mtab (getmntent) to enumerate mounts
    FILE* mtab = setmntent("/etc/mtab", "r");
    if (!mtab) {
        mtab = setmntent("/proc/mounts", "r"); // fallback
    }
    if (!mtab) return info;

    struct mntent* ent;
    while ((ent = getmntent(mtab)) != nullptr) {
        const char* dev = ent->mnt_fsname ? ent->mnt_fsname : "";
        const char* mnt = ent->mnt_dir ? ent->mnt_dir : "";
        const char* fstype = ent->mnt_type ? ent->mnt_type : "";
        int flags = ent->mnt_opts ? 0 : 0;

        // skip pseudo filesystems to avoid noise
        std::string fs(fstype);
        if (fs == "proc" || fs == "sysfs" || fs == "tmpfs" || fs == "devtmpfs" ||
            fs == "devpts" || fs == "cgroup" || fs == "securityfs" || fs == "pstore" ||
            fs == "tracefs" || fs == "efivarfs" || fs == "rpc_pipefs" || fs == "binfmt_misc")
            continue;

        struct statvfs st;
        if (statvfs(mnt, &st) != 0) {
            continue;
        }

        VolumeInfo v;
        v.device = dev;
        v.mountPoint = mnt;
        v.fsType = fstype;
        v.readOnly = (ent->mnt_opts && std::string(ent->mnt_opts).find("ro") != std::string::npos);

        v.totalBytes = static_cast<uint64_t>(st.f_frsize) * static_cast<uint64_t>(st.f_blocks);
        v.freeBytes = static_cast<uint64_t>(st.f_frsize) * static_cast<uint64_t>(st.f_bfree);
        v.availableBytes = static_cast<uint64_t>(st.f_frsize) * static_cast<uint64_t>(st.f_bavail);

        // try to detect removable: if device path contains /dev/sdX or /dev/mmcblk or /dev/nvme
        std::string devs = dev;
        if (devs.find("/dev/") != std::string::npos) {
            if (devs.find("sd") != std::string::npos || devs.find("mmcblk") != std::string::npos || devs.find("nvme") != std::string::npos)
                v.removable = false; // most block devices are non-removable; more checks can be added
        } else {
            // e.g. tmpfs, cgroup - not removable
            v.removable = false;
        }

        info.volumes.push_back(v);
    }

    endmntent(mtab);
    return info;

#elif defined(__APPLE__)
    // macOS: use getmntinfo
    struct statfs* mounts = nullptr;
    int count = getmntinfo(&mounts, MNT_NOWAIT);
    if (count <= 0) return info;

    for (int i = 0; i < count; ++i) {
        struct statfs& m = mounts[i];

        std::string fsname = m.f_fstypename ? m.f_fstypename : "";
        // skip certain pseudo FS if desired
        if (fsname == "devfs" || fsname == "autofs" || fsname == "proc" ) continue;

        VolumeInfo v;
        v.device = m.f_mntfromname ? m.f_mntfromname : "";
        v.mountPoint = m.f_mntonname ? m.f_mntonname : "";
        v.fsType = fsname;
        v.readOnly = (m.f_flags & MNT_RDONLY);

        uint64_t blockSize = static_cast<uint64_t>(m.f_bsize);
        v.totalBytes = blockSize * static_cast<uint64_t>(m.f_blocks);
        v.freeBytes = blockSize * static_cast<uint64_t>(m.f_bfree);
        v.availableBytes = blockSize * static_cast<uint64_t>(m.f_bavail);

        // removable detection: if device path starts with /dev/disk (could be external)
        if (v.device.find("/dev/disk") == 0) {
            // further checks could be done via IOKit, but mark false by default
            v.removable = false;
        }

        info.volumes.push_back(v);
    }

    return info;

#else
    return info;
#endif
}

void StorageInfo::Print() const {
    std::cout << "=== Storage Volumes ===\n";
    for (const auto& v : volumes) {
        if (v.freeMB() > 0){
            std::cout << "Device:      " << v.device << "\n";
            std::cout << "Mount:       " << v.mountPoint << "\n";
            std::cout << "FS:          " << v.fsType << (v.readOnly ? " (ro)" : "") << "\n";
            std::cout << "Total:       " << v.totalMB() << " MB\n";
            std::cout << "Free:        " << v.freeMB() << " MB\n";
            std::cout << "Available:   " << v.availMB() << " MB\n";
            std::cout << "Removable:   " << (v.removable ? "Yes" : "No") << "\n";
            std::cout << "---------------------------\n";
        }
    }
    std::cout << "=========================\n";
}
