#include <fstream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <thread>
#include "FileSystem.hpp"

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <direct.h>
#define getcwd _getcwd
#define mkdir(path) _mkdir(path)
#else
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h>
#endif

namespace FileSystem
{
    // PATH
#ifdef PLATFORM_WINDOWS
    const char Path::Separator = '\\';
#else
    const char Path::Separator = '/';
#endif

    std::string Path::Normalize(const std::string& path) 
    {
        std::string result =path;
        std::replace(result.begin(), result.end(),
            Separator == '/' ? '\\' : '/',
            Separator);

        return result;
    }

    std::string Path::Join(const std::string& a, const std::string& b) 
    {
        if (a.empty()) return b;
        if (b.empty()) return a;

        std::string result = a;
        if (result.back() != Separator && result.back() != '/' && result.back() != '\\')
        {
            result += Separator;
        }
        result += b;

        return Normalize(result);
    }

    std::string Path::GetDirectory(const std::string& path)
    {
        size_t pos = path.find_last_of("/\\");
        if (pos == std::string::npos) return "";
        return path.substr(0, pos);
    }

    std::string Path::GetFilename(const std::string& path) 
    {
        size_t pos = path.find_last_of("/\\");
        if (pos == std::string::npos) return path;
        return path.substr(pos + 1);
    }

    std::string Path::GetExtension(const std::string& path) 
    {
        std::string filename = GetFilename(path);
        size_t pos = filename.find_last_of('.');
        if (pos == std::string::npos) return "";
        return filename.substr(pos);
    }

    std::string Path::GetFilenameWithoutExtension(const std::string& path) 
    {
        std::string filename = GetFilename(path);
        size_t pos = filename.find_last_of('.');
        if (pos == std::string::npos) return filename;
        return filename.substr(0, pos);
    }

    bool Path::IsAbsolute(const std::string& path) 
    {
    #ifdef PLATFORM_WINDOWS
        return path.length() >= 2 && path[1] == ':';
    #else
        return !path.empty() && path[0] == '/';
    #endif
    }


    // File

    bool File::Exists(const std::string& path) 
    {
        std::ifstream file(path);
        return file.good();
    }

    bool File::IsFile(const std::string& path) 
    {
    #ifdef PLATFORM_WINDOWS
        DWORD attr = GetFileAttributesA(path.c_str());
        return (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
    #else
        struct stat st;
        if (stat(path.c_str(), &st) != 0) return false;
        return S_ISREG(st.st_mode);
    #endif
    }

    bool File::IsDirectory(const std::string& path) 
    {
    #ifdef PLATFORM_WINDOWS
        DWORD attr = GetFileAttributesA(path.c_str());
        return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY));
    #else
        struct stat st;
        if (stat(path.c_str(), &st) != 0) return false;
        return S_ISDIR(st.st_mode);
    #endif
    }

    uint64_t File::GetSize(const std::string& path) 
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file) return 0;
        return static_cast<uint64_t>(file.tellg());
    }

    uint64_t File::GetLastModifiedTime(const std::string& path) 
    {
    #ifdef PLATFORM_WINDOWS
        WIN32_FILE_ATTRIBUTE_DATA data;
        if (!GetFileAttributesExA(path.c_str(), GetFileExInfoStandard, &data)) {
            return 0;
        }
        ULARGE_INTEGER ull;
        ull.LowPart = data.ftLastWriteTime.dwLowDateTime;
        ull.HighPart = data.ftLastWriteTime.dwHighDateTime;
        return ull.QuadPart;
    #else
        struct stat st;
        if (stat(path.c_str(), &st) != 0) return 0;
        return static_cast<uint64_t>(st.st_mtime);
    #endif
    }

    std::vector<uint8_t> File::ReadBinary(const std::string& path) 
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file) {
            return {};
        }
        
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> buffer(size);
        file.read(reinterpret_cast<char*>(buffer.data()), size);
        
        return buffer;
    }

    std::string File::ReadText(const std::string& path) 
    {
        std::ifstream file(path);
        if (!file) {
            return "";
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    std::vector<std::string> File::ReadLines(const std::string& path) 
    {
        std::ifstream file(path);
        if (!file) {
            return {};
        }
        
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        
        return lines;
    }

    bool File::WriteBinary(const std::string& path, const void* data, size_t size) 
    {
        std::ofstream file(path, std::ios::binary);
        if (!file) return false;
        
        file.write(static_cast<const char*>(data), size);
        return file.good();
    }

    bool File::WriteText(const std::string& path, const std::string& text) 
    {
        std::ofstream file(path);
        if (!file) return false;
        
        file << text;
        return file.good();
    }

    bool File::AppendText(const std::string& path, const std::string& text) 
    {
        std::ofstream file(path, std::ios::app);
        if (!file) return false;
        
        file << text;
        return file.good();
    }

    bool File::Delete(const std::string& path) 
    {
        return std::remove(path.c_str()) == 0;
    }

    bool File::Copy(const std::string& src, const std::string& dst) 
    {
        std::ifstream srcFile(src, std::ios::binary);
        std::ofstream dstFile(dst, std::ios::binary);
        
        if (!srcFile || !dstFile) return false;
        
        dstFile << srcFile.rdbuf();
        return true;
    }

    bool File::Move(const std::string& src, const std::string& dst) 
    {
        return std::rename(src.c_str(), dst.c_str()) == 0;
    }

    bool File::Rename(const std::string& oldPath, const std::string& newPath) 
    {
        return Move(oldPath, newPath);
    }


    // Directory Implementation

    bool Directory::Create(const std::string& path) 
    {
    #ifdef PLATFORM_WINDOWS
        return _mkdir(path.c_str()) == 0;
    #else
        return mkdir(path.c_str(), 0755) == 0;
    #endif
    }

    bool Directory::CreateRecursive(const std::string& path) 
    {
        if (Exists(path)) return true;
        
        std::string parent = Path::GetDirectory(path);
        if (!parent.empty() && !Exists(parent)) {
            if (!CreateRecursive(parent)) return false;
        }
        
        return Create(path);
    }

    bool Directory::Delete(const std::string& path, bool recursive) 
    {
        if (!Exists(path)) return false;
        
        if (recursive) {
            auto files = ListFiles(path);
            for (const auto& file : files) {
                File::Delete(Path::Join(path, file));
            }
            
            auto dirs = ListDirectories(path);
            for (const auto& dir : dirs) {
                Delete(Path::Join(path, dir), true);
            }
        }
        
    #ifdef PLATFORM_WINDOWS
        return _rmdir(path.c_str()) == 0;
    #else
        return rmdir(path.c_str()) == 0;
    #endif
    }

    bool Directory::Exists(const std::string& path) 
    {
        return File::IsDirectory(path);
    }

    std::vector<std::string> Directory::ListFiles(const std::string& path) 
    {
        std::vector<std::string> files;
        
    #ifdef PLATFORM_WINDOWS
        WIN32_FIND_DATAA findData;
        HANDLE hFind = FindFirstFileA((path + "\\*").c_str(), &findData);
        
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    files.push_back(findData.cFileName);
                }
            } while (FindNextFileA(hFind, &findData));
            FindClose(hFind);
        }
    #else
        DIR* dir = opendir(path.c_str());
        if (dir) {
            struct dirent* entry;
            while ((entry = readdir(dir)) != nullptr) {
                std::string fullPath = Path::Join(path, entry->d_name);
                if (File::IsFile(fullPath)) {
                    files.push_back(entry->d_name);
                }
            }
            closedir(dir);
        }
    #endif
        
        return files;
    }

    std::vector<std::string> Directory::ListDirectories(const std::string& path) 
    {
        std::vector<std::string> dirs;
        
    #ifdef PLATFORM_WINDOWS
        WIN32_FIND_DATAA findData;
        HANDLE hFind = FindFirstFileA((path + "\\*").c_str(), &findData);
        
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                    strcmp(findData.cFileName, ".") != 0 &&
                    strcmp(findData.cFileName, "..") != 0) {
                    dirs.push_back(findData.cFileName);
                }
            } while (FindNextFileA(hFind, &findData));
            FindClose(hFind);
        }
    #else
        DIR* dir = opendir(path.c_str());
        if (dir) {
            struct dirent* entry;
            while ((entry = readdir(dir)) != nullptr) {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                    continue;
                }
                std::string fullPath = Path::Join(path, entry->d_name);
                if (File::IsDirectory(fullPath)) {
                    dirs.push_back(entry->d_name);
                }
            }
            closedir(dir);
        }
    #endif
        
        return dirs;
    }

    std::vector<std::string> Directory::ListAll(const std::string& path) 
    {
        auto files = ListFiles(path);
        auto dirs = ListDirectories(path);
        files.insert(files.end(), dirs.begin(), dirs.end());
        return files;
    }

    std::string Directory::GetCurrent() 
    {
        char buffer[1024];
        if (getcwd(buffer, sizeof(buffer)) != nullptr) {
            return std::string(buffer);
        }
        return "";
    }

    std::string Directory::GetExecutable() 
    {
    #ifdef PLATFORM_WINDOWS
        char buffer[MAX_PATH];
        GetModuleFileNameA(nullptr, buffer, MAX_PATH);
        return Path::GetDirectory(buffer);
    #else
        char buffer[1024];
        ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
        if (len != -1) {
            buffer[len] = '\0';
            return Path::GetDirectory(buffer);
        }
        return GetCurrent();
    #endif
    }

    std::string Directory::GetUserHome() 
    {
    #ifdef PLATFORM_WINDOWS
        char* userProfile = getenv("USERPROFILE");
        return userProfile ? userProfile : "";
    #else
        char* home = getenv("HOME");
        return home ? home : "";
    #endif
    }

    std::string Directory::GetTemp() 
    {
    #ifdef PLATFORM_WINDOWS
        char buffer[MAX_PATH];
        GetTempPathA(MAX_PATH, buffer);
        return buffer;
    #else
        return "/tmp";
    #endif
    }


    // AsyncFile Implementation

    void AsyncFile::ReadBinaryAsync(const std::string& path, 
                                    ReadCallback onSuccess,
                                    ErrorCallback onError) 
    {
        std::thread([path, onSuccess, onError]() {
            try {
                auto data = File::ReadBinary(path);
                if (data.empty() && onError) {
                    onError("Failed to read file: " + path);
                } else if (onSuccess) {
                    onSuccess(std::move(data));
                }
            } catch (const std::exception& e) {
                if (onError) {
                    onError(std::string("Exception: ") + e.what());
                }
            }
        }).detach();
    }
}

