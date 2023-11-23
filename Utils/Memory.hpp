#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <sys/uio.h>
#include <math.h>
namespace fs = std::filesystem;

namespace Memory {
    pid_t PID = 0;

    pid_t GetPID() {
        if (PID > 0)
            return PID;

        for (const auto& Entry : fs::directory_iterator("/proc")) {
            if (!Entry.is_directory())
                continue;

              std::ifstream CommandFile(std::string(Entry.path()) + "/cmdline");
              std::string CommandLine;
              std::getline(CommandFile, CommandLine);

              if (CommandLine.find("r5apex.exe") != std::string::npos) {
                  PID = std::stoi(Entry.path().filename());
                  break;
              }

        }
        return PID;
    }

    bool IsValidPointer(long Pointer) {
        return Pointer > 0x00010000 && Pointer < 0x7FFFFFFEFFFF;
    }

    bool Read(long address, void *pBuff, size_t size) {
        if (size == 0)
            return false;
        void *pAddress = (void *)address;
        pid_t pid = GetPID();
        struct iovec iovLocalAddressSpace[1]{0};
        struct iovec iovRemoteAddressSpace[1]{0};
        iovLocalAddressSpace[0].iov_base = pBuff;     // Store data in this buffer
        iovLocalAddressSpace[0].iov_len = size;       // which has this size.
        iovRemoteAddressSpace[0].iov_base = pAddress; // The data comes from here
        iovRemoteAddressSpace[0].iov_len = size;      // and has this size.
        ssize_t sSize = process_vm_readv(
            pid,                   // Remote process id
            iovLocalAddressSpace,  // Local iovec array
            1,                     // Size of the local iovec array
            iovRemoteAddressSpace, // Remote iovec array
            1,                     // Size of the remote iovec array
            0);                    // Flags, unused
        if (sSize == (ssize_t)size)
            return true;
        else if (sSize == 0)
            return false;
        return false;
    }

    bool Write(long address, void *pBuff, size_t size) {
        if (size == 0)
            return false;
        void *pAddress = (void *)address;
        pid_t pid = GetPID();
        struct iovec iovLocalAddressSpace[1]{0};
        struct iovec iovRemoteAddressSpace[1]{0};
        iovLocalAddressSpace[0].iov_base = pBuff;     // Store data in this buffer
        iovLocalAddressSpace[0].iov_len = size;       // which has this size.
        iovRemoteAddressSpace[0].iov_base = pAddress; // The data will be writted here
        iovRemoteAddressSpace[0].iov_len = size;      // and has this size.
        ssize_t sSize = process_vm_writev(
            pid,                   // Remote process id
            iovLocalAddressSpace,  // Local iovec array
            1,                     // Size of the local iovec array
            iovRemoteAddressSpace, // Remote iovec array
            1,                     // Size of the remote iovec array
            0);                    // Flags, unused
        if (sSize == (ssize_t)size)
            return true;
        else if (sSize == 0)
            return false;
        return false;
    }

    template <class T>
    T Read(long Address) {
        T buffer;
        bool success = Read(Address, &buffer, sizeof(T));
        if (!success)
        {
            PID = 0;
            throw std::invalid_argument(
                "Failed to get " + std::to_string(sizeof(T)) + "at: " + std::to_string(Address));
        }
        return buffer;
    }

    template <class T>
    void Write(long Address, T Value) {
        bool success = Write(Address, &Value, sizeof(T));
        if (!success) {
            PID = 0;
            throw std::invalid_argument(
                "Failed to set " + std::to_string(sizeof(T)) + " at: " + std::to_string(Address));
        }
    }

    std::string ReadString(long address) {
        int size = sizeof(std::string);
        char buffer[size] = {0};
        bool success = Read(address, &buffer, size);
        if (!success)
            throw new std::invalid_argument("Failed to read string at address: " + address);
        return std::string(buffer);
    }

    std::string ReadPlayerName(long address, int size) {
        char buffer[size] = { 0 };
        bool success = Read(address, &buffer, size);
        if (!success)
            throw new std::invalid_argument("Failed to read Player Name at address: " + address);
        return std::string(buffer);
    }

    std::string ConvertPointerToHexString(long pointer) {
        std::stringstream stream;
        stream << "0x" << std::hex << pointer;
        std::string result(stream.str());
        return result;
    }
}
