#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include <string>
#include <cstdint>

#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
    #ifdef _WIN64
        #define PLATFORM_64BIT
    #else
        #define PLATFORM_32BIT
    #endif
#elif defined(__linux__)
    #define PLATFORM_LINUX
    #if defined(__x86_64__) || defined(__aarch64__)
        #define PLATFORM_64BIT
    #else
        #define PLATFORM_32BIT
    #endif
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_MAC
        #define PLATFORM_MACOS
    #endif
    #define PLATFORM_64BIT
#else
    #define PLATFORM_UNKNOWN
#endif

#if defined(_MSC_VER)
    #define COMPILER_MSVC
#elif defined(__GNUC__) || defined(__GNUG__)
    #define COMPILER_GCC
#elif defined(__clang__)
    #define COMPILER_CLANG
#endif

#if defined(_DEBUG) || defined(DEBUG)
    #define BUILD_DEBUG
#else
    #define BUILD_RELEASE
#endif

namespace Platform
{
    enum class OS {
        Windows,
        Linux,
        MacOS,
        Unknown
    };

    enum class Architecture {
        x86,
        x64,
        ARM,
        ARM64,
        Unknown
    };

    enum class Compiler {
        MSVC,
        GCC,
        Clang,
        Unknown,
    };

    enum class BuildType {
        Debug,
        Release
    };

    class Info
    {
    public:
        static OS GetOS();
        static Architecture GetArchitecture();
        static Compiler GetCompiler();
        static BuildType GetBuildType();

        static std::string GetOSName();
        static std::string GetArchitectureName();
        static std::string GetCompilerName();

        static bool IsWindows();
        static bool IsLinux();
        static bool IsMacOS();
        static bool IsDebug();
        static bool Is64Bit();

        static void Print();
    };
}



#endif // !PLATFORM_HPP