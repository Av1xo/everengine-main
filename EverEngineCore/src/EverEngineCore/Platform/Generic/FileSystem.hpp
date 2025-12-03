#ifndef FILE_SYSTEM_HPP
#define FILE_SYSTEM_HPP

#include <string>
#include <vector>
#include <functional>
#include <cstdint>

namespace FileSystem
{
    class Path
    {
    public:
        static std::string Normalize(const std::string& path);
        static std::string Join(const std::string& a, const std::string&  b);
        static std::string GetDirectory(const std::string&  path);
        static std::string GetFilename(const std::string& path);
        static std::string GetExtension(const std::string& path);
        static std::string GetFilenameWithoutExtension(const std::string& path);
        static bool IsAbsolute(const std::string& path);

        static const char Separator;
    };

    class File
    {
    public:
        static bool Exists(const std::string& path);
        static bool IsFile(const std::string& path);
        static bool IsDirectory(const std::string& path);

        static uint64_t GetSize(const std::string& path);
        static uint64_t GetLastModifiedTime(const std::string& path);

        static std::vector<uint8_t> ReadBinary(const std::string& path);
        static std::string ReadText(const std::string& path);
        static std::vector<std::string> ReadLines(const std::string& path);

        static bool WriteBinary(const std::string& path, const void* data, size_t size);
        static bool WriteText(const std::string& path, const std::string& data);
        static bool AppendText(const std::string& path, const std::string& data);

        static bool Delete(const std::string& path);
        static bool Copy(const std::string& src, const std::string& dst);
        static bool Move(const std::string& src, const std::string& dst);
        static bool Rename(const std::string& oldPath, const std::string& newPath);
    };

    class Directory
    {
    public:
        static bool Create(const std::string& path);
        static bool CreateRecursive(const std::string& path);
        static bool Delete(const std::string& path, bool recursive = false);
        static bool Exists(const std::string& path);

        static std::vector<std::string> ListFiles(const std::string& path);
        static std::vector<std::string> ListDirectories(const std::string& path);
        static std::vector<std::string> ListAll(const std::string& path);

        static std::string GetCurrent();
        static std::string GetExecutable();
        static std::string GetUserHome();
        static std::string GetTemp();
    };

    class AsyncFile
    {
    public:
        using ReadCallback = std::function<void(std::vector<uint8_t>)>;
        using ErrorCallback = std::function<void(const std::string&)>;

        static void ReadBinaryAsync(const std::string& path,
            ReadCallback onSuccess,
            ErrorCallback onError = nullptr);
    };
}

#endif