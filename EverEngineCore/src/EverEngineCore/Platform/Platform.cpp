#include <iostream>
#include "Platform.hpp"

namespace Platform
{
    OS Info::GetOS()
    {
    #ifdef PLATFORM_WINDOWS
        return OS::Windows;
    #elif defined(PLATFORM_LINUX)
        return OS::Linux;
    #elif defined(PLATFORM_MACOS)
        return OS::MacOS;
    #else
        return OS::Unknown;
    #endif
    }

    Architecture Info::GetArchitecture() 
    {
    #if defined(__x86_64__) || defined(_M_X64)
        return Architecture::x64;
    #elif defined(__i386__) || defined(_M_IX86)
        return Architecture::x86;
    #elif defined(__aarch64__) || defined(_M_ARM64)
        return Architecture::ARM64;
    #elif defined(__arm__) || defined(_M_ARM)
        return Architecture::ARM;
    #else
        return Architecture::Unknown;
    #endif
    }

    Compiler Info::GetCompiler() 
    {
    #ifdef COMPILER_MSVC
        return Compiler::MSVC;
    #elif defined(COMPILER_CLANG)
        return Compiler::Clang;
    #elif defined(COMPILER_GCC)
        return Compiler::GCC;
    #else
        return Compiler::Unknown;
    #endif
    }

    BuildType Info::GetBuildType() 
    {
    #ifdef BUILD_DEBUG
        return BuildType::Debug;
    #else
        return BuildType::Release;
    #endif
    }

    std::string Info::GetOSName() 
    {
        switch (GetOS()) 
        {
            case OS::Windows: return "Windows";
            case OS::Linux: return "Linux";
            case OS::MacOS: return "macOS";
            default: return "Unknown";
        }
    }
    std::string Info::GetArchitectureName() 
    {
        switch (GetArchitecture()) 
        {
            case Architecture::x86:   return "x86 (32-bit)";
            case Architecture::x64:   return "x64 (64-bit)";
            case Architecture::ARM:   return "ARM (32-bit)";
            case Architecture::ARM64: return "ARM64 (64-bit)";
            default:                  return "Unknown";
        }
    }

    std::string Info::GetCompilerName() 
    {
        switch (GetCompiler()) {
            case Compiler::MSVC:  return "MSVC";
            case Compiler::GCC:   return "GCC";
            case Compiler::Clang: return "Clang";
            default:              return "Unknown";
        }
    }

    bool Info::IsWindows() { return GetOS() == OS::Windows; }
    bool Info::IsLinux()   { return GetOS() == OS::Linux; }
    bool Info::IsMacOS()   { return GetOS() == OS::MacOS; }
    bool Info::IsDebug()   { return GetBuildType() == BuildType::Debug; }
    bool Info::Is64Bit()   
    { 
        auto arch = GetArchitecture();
        return arch == Architecture::x64 || arch == Architecture::ARM64;
    }

    void Info::Print() 
    {
        std::cout << "=== Platform Information ===" << std::endl;
        std::cout << "OS: " << GetOSName() << std::endl;
        std::cout << "Architecture: " << GetArchitectureName() << std::endl;
        std::cout << "Compiler: " << GetCompilerName() << std::endl;
        std::cout << "Build: " << (IsDebug() ? "Debug" : "Release") << std::endl;
        std::cout << "===========================" << std::endl;
    }
}