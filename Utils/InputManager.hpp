#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <thread>
#include <fcntl.h>
#include <linux/input.h>
#include <cstring>
#include <libudev.h>
#include <unistd.h>
#include <cstdio>
#include <stdlib.h>
#include <string.h>

#include "InputTypes.hpp"

#define test_bit(yalv, abs_b) ((((char *)abs_b)[yalv/8] & (1<<yalv%8)) > 0)

struct InputDeviceHandler{
private:
    int _fd;

public:
    bool openDevice(const char* deviceName) {
        _fd = open(deviceName, O_RDONLY | O_NONBLOCK);
        if (_fd <= 0) {
            return false;
        }

        return true;
    }

    void closeDevice() {
        close(_fd);
    }

    bool readEv(input_event& ie) {
        int bytes = read(_fd, &ie, sizeof(ie));
        if (bytes > 0) {
            if (ie.type & EV_KEY) {
                return true;
            }
        }

        return false;
    }
};

class InputManager {
private:
    static ushort _inputTypes[InputKeyTypeCount];
    static std::vector<InputDeviceHandler> _keyboardInputHandlers;
    static std::vector<InputDeviceHandler> _mouseInputHandlers;

    static void readEv() {
        input_event ie;
        
        for(auto keyboardInputHandler : _keyboardInputHandlers) {
            ie = {0};
            if(keyboardInputHandler.readEv(ie)) {  
                _inputTypes[static_cast<int>(mapKeyboardKeyType(ie.code))] = ie.value;
                //std::cout << (int)mapKeyboardKeyType(ie.code) << " " << ie.value  << std::endl;
            }
        }

        for(auto mouseInputHandler : _mouseInputHandlers) {
            ie = {0};
            if(mouseInputHandler.readEv(ie)) {
                _inputTypes[static_cast<int>(mapMouseButtonType(ie.code))] = ie.value;
                //std::cout << (int)mapMouseButtonType(ie.code) << " " << ie.value << std::endl;
            }
        }
    }

    static void closeDevices() {
        for(auto keyboardInputHandler : _keyboardInputHandlers) {
            keyboardInputHandler.closeDevice();
        }

        for(auto mouseInputHandler : _mouseInputHandlers) {
            mouseInputHandler.closeDevice();
        }
    }

    static bool isValidInputDevice(const char* device) {
        if(device == nullptr || std::string(device).find("event") == std::string::npos) {
            return false;
        }

        int fd = open(device, O_RDONLY | O_NONBLOCK);
        if (fd <= 0) {
            return false;
        }

        unsigned char evtype_b[(EV_MAX + 7) / 8];
        ioctl(fd, EVIOCGBIT(0, sizeof(evtype_b)), evtype_b);

        close(fd);

        if (!test_bit(EV_KEY, evtype_b)) {
            return false;
        }
        
        return true;
    }

    static std::vector<std::string> listInputDevices(const char* deviceType) {
        std::vector<std::string> result;

        udev* udevContext = udev_new();
        if (!udevContext) {
            std::cerr << "Failed to create udev context" << std::endl;
            return result;
        }

        udev_enumerate* udevEnumerate = udev_enumerate_new(udevContext);
        if (!udevEnumerate) {
            std::cerr << "Failed to create udev enumeration context\n";
            udev_unref(udevContext);
            return result;
        }

        udev_enumerate_add_match_subsystem(udevEnumerate, "input");
        udev_enumerate_add_match_property(udevEnumerate, deviceType, "1");
        udev_enumerate_scan_devices(udevEnumerate);

        udev_list_entry* devicesList = udev_enumerate_get_list_entry(udevEnumerate);
        udev_list_entry* device = nullptr;

        udev_list_entry_foreach(device, devicesList) {
            const char* path = udev_list_entry_get_name(device);
            udev_device* dev = udev_device_new_from_syspath(udevContext, path);
            if (dev) {
                const char* devnode = udev_device_get_devnode(dev);
                if (isValidInputDevice(devnode)) {
                    result.push_back(std::string(devnode));
                }
                udev_device_unref(dev);
            }
        }

        udev_enumerate_unref(udevEnumerate);
        udev_unref(udevContext);

        return result;
    }

    static void addKeyboardHandler(std::string device) {
        InputDeviceHandler keyboardInputHandler;
        if(!keyboardInputHandler.openDevice(device.c_str())) {
            return;
        }

        //std::cout << "Found keyboard device node: " << device << std::endl;
        _keyboardInputHandlers.push_back(keyboardInputHandler);
    }

    static bool initKeyboard() {
        
        auto devices = listInputDevices("ID_INPUT_KEYBOARD");
        
        if(devices.empty()) {
            return false;
        }

        for(auto device : devices) {
            addKeyboardHandler(device);
        }

        if(_keyboardInputHandlers.empty()) {
            std::cout << "Failed to find keyborad device. Input manager will not work." << std::endl;
            return false;
        }

        return true;
    }

    static void addMouseHandler(std::string device) {
        InputDeviceHandler mouseInputHandler;
        if(!mouseInputHandler.openDevice(device.c_str())) {
            return;
        }

        //std::cout << "Found mouse device node: " << device << std::endl;
        _mouseInputHandlers.push_back(mouseInputHandler);
    }

    static bool initMouse() {
        
        auto devices = listInputDevices("ID_INPUT_MOUSE");
        
        if(devices.empty()) {
            return false;
        }

        for(auto device : devices) {
            addMouseHandler(device);
        }

        if(_mouseInputHandlers.empty()) {
            std::cout << "Failed to find mouse device. Input manager will not work." << std::endl;
            return false;
        }

        return true;
    }

    static bool init() {
        if(!initKeyboard()) {
            return false;
        }

        if(!initMouse()) {
            return false;
        }

        return true;
    }

public:

    static std::atomic_bool StopThread;

    static void run() {

        if(!init())
        {
            std::cout << "Failed to init devices. Input manager will not work." << std::endl;
            return;
        }

        while (!StopThread)
        {
            readEv();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        closeDevices();
    }

    static bool isKeyDownOrPress(InputKeyType key) {
        return _inputTypes[static_cast<int>(key)] > 0;
    }

    static bool isKeyDown(InputKeyType key) {
        return _inputTypes[static_cast<int>(key)] == 1;
    }

    static bool isKeyPress(InputKeyType key) {
        return _inputTypes[static_cast<int>(key)] == 2;
    }

    static bool isKeyUp(InputKeyType key) {
        return _inputTypes[static_cast<int>(key)] == 0;
    }
};

ushort InputManager::_inputTypes[InputKeyTypeCount] = {0};
std::atomic_bool InputManager::StopThread(false);
std::vector<InputDeviceHandler> InputManager::_keyboardInputHandlers = std::vector<InputDeviceHandler>();
std::vector<InputDeviceHandler> InputManager::_mouseInputHandlers = std::vector<InputDeviceHandler>();