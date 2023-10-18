// Externals
#include <iostream>
#include <string>
#include <unistd.h>
#include <atomic>
#include <vector>
#include <chrono>
#include <thread>
#include <GLFW/glfw3.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XInput2.h>

// Internals
#include "Core/Level.hpp"
#include "Core/Player.hpp"
#include "Core/LocalPlayer.hpp"

#include "Features/Aimbot.hpp"
#include "Features/Sense.hpp"
#include "Features/Triggerbot.hpp"

#include "Overlay/Overlay.hpp"

#include "Utils/Config.hpp"
#include "Utils/Memory.hpp"
#include "Utils/XDisplay.hpp"

// Objects
XDisplay* X11Display = new XDisplay();
Overlay OverlayWindow = Overlay();

// Game Objects
Level* Map = new Level();
LocalPlayer* Myself = new LocalPlayer();

// Players
std::vector<Player*>* HumanPlayers = new std::vector<Player*>;
std::vector<Player*>* Dummies = new std::vector<Player*>;
std::vector<Player*>* Players = new std::vector<Player*>;

// Features
Sense* ESP = new Sense(Players);
Aimbot* AimAssist = new Aimbot(X11Display, Myself, Players);
Triggerbot* Trigger = new Triggerbot(X11Display, Myself, Players);

// Booleans and Variables
bool IsMenuOpened = true;

// Thread
std::atomic_bool StopThread(false);

// Inputs
void HandleKeyEvent(Display* display, XEvent* Event) {
    if (Event->type == KeyPress) {
        if (IsMenuOpened) {
            IsMenuOpened = false;
            OverlayWindow.CaptureInput(false);
        } else {
            IsMenuOpened = true;
            OverlayWindow.CaptureInput(true);
        }
    }
}

void X11EventListener() {
    Display* display = XOpenDisplay(nullptr);
    Window root = DefaultRootWindow(display);
    XGrabKey(display, XKeysymToKeycode(display, XK_Insert), AnyModifier, root, False, GrabModeAsync, GrabModeAsync);
    XEvent event;
    while (!StopThread) {
        XNextEvent(display, &event);
        HandleKeyEvent(display, &event);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    XCloseDisplay(display);
}

// Overlay
bool InitializeOverlayWindow() {
    if (!OverlayWindow.InitializeOverlay()) {
        OverlayWindow.DestroyOverlay();
        return false;
    }
    int ScreenWidth;
    int ScreenHeight;
    OverlayWindow.GetScreenResolution(ScreenWidth, ScreenHeight);
    std::cout << "overlay initialized" << std::endl;
    return true;
}

// Config
void LoadConfig() {
    bool success = ReadConfig("config.ini");
    if (success)
        std::cout << "successfully read config" << std::endl;
    else
        std::cout << "can't read config for some reason so new config file has been created" << std::endl;

    // Aimbot //
    AimAssist->AimbotEnabled = Config::Aimbot::Enabled;
    AimAssist->AllowTargetSwitch = Config::Aimbot::AllowTargetSwitch;
    AimAssist->PredictMovement = Config::Aimbot::PredictMovement;
    AimAssist->PredictBulletDrop = Config::Aimbot::PredictBulletDrop;
    AimAssist->Hitbox = static_cast<HitboxType>(Config::Aimbot::Hitbox);
    AimAssist->Speed = Config::Aimbot::Speed;
    AimAssist->Smooth = Config::Aimbot::Smooth;
    AimAssist->ExtraSmooth = Config::Aimbot::ExtraSmooth;
    AimAssist->MaxFOV = Config::Aimbot::FOV;
    AimAssist->MinDistance = Config::Aimbot::MinDistance;
    AimAssist->MaxDistance = Config::Aimbot::MaxDistance;

    // ESP //
    ESP->GlowEnabled = Config::Glow::Enabled;
    ESP->ItemGlow = Config::Glow::ItemGlow;
    ESP->GlowMaxDistance = Config::Glow::MaxDistance;

    // Triggerbot //
    Trigger->TriggerbotEnabled = Config::Triggerbot::Enabled;
    Trigger->TriggerbotRange = Config::Triggerbot::Range;
}

void SaveConfig() {
    if (!AimAssist->Save()) std::cout << "something went wrong trying to save Aimbot settings" << std::endl;
    if (!ESP->Save()) std::cout << "something went wrong trying to save ESP settings" << std::endl;
    if (!Trigger->Save()) std::cout << "something went wrong trying to save Triggerbot settings" << std::endl;
    UpdateConfig();
}

// Interface
ImVec4 ProcessingTimeColor;
void RenderUI() {
    if (!IsMenuOpened) return;

    // Menu
    ImGui::SetNextWindowSizeConstraints(ImVec2(440, 420), ImVec2(440, 420));
    ImGui::SetNextWindowSize(ImVec2(440, 420), ImGuiCond_FirstUseEver);
    ImGui::Begin(" ", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
    
    ProcessingTimeColor = OverlayWindow.ProcessingTime > 20 ? ProcessingTimeColor = ImVec4(1, 0.343, 0.475, 1) : ProcessingTimeColor = ImVec4(0.4, 1, 0.343, 1);
    ImGui::TextColored(ProcessingTimeColor, "Processing Time: %02dms", OverlayWindow.ProcessingTime);
    ImGui::SameLine();

    if (OverlayWindow.AlignedButton("Save Config", 1.0f)) {
        SaveConfig();
        std::cout << "config saved" << std::endl;
    }    

    if (ImGui::BeginTabBar("Menus"), ImGuiTabBarFlags_NoCloseWithMiddleMouseButton) {
        // Draw Settings //
        AimAssist->RenderUI();
        Trigger->RenderUI();
        ESP->RenderUI();

        // Draw Credits //
        if (ImGui::BeginTabItem("Credits", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton | ImGuiTabItemFlags_NoReorder)) {
            ImGui::TextColored(ImVec4(0.65, 1, 0.95, 1), "Credits:");
            ImGui::TextColored(ImVec4(0.03, 1, 0.95, 1), "arturzxc: ESP, Triggerbot and X11Display's Move Mouse method");
            ImGui::TextColored(ImVec4(0.3, 1, 0.64, 1), "Koelion: ImGui Menu, AimbotResolver");
            ImGui::TextColored(ImVec4(0.3, 0.6, 0.9, 1), "unknowncheats: Offsets and ton of other things");
            ImGui::TextColored(ImVec4(0.6, 1, 0.64, 1), "Made by Azreol/Nexilist");
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

// Core
bool UpdateCore() {
    try {
        // Map Checking //
        Map->Read();
        if (!Map->IsPlayable) {
            return true;
        }

        // Read Local Player //
        Myself->Read();
        if (!Myself->IsCombatReady()) {
            return true;
        }

        // Populate Players //
        Players->clear();
        if (Map->IsFiringRange) {
            for (int i = 0; i < Dummies->size(); i++) {
                Player* p = Dummies->at(i);
                p->Read();
                if (p->IsValid())
                    Players->push_back(p);
            }
        } else {
            for (int i = 0; i < HumanPlayers->size(); i++) {
                Player* p = HumanPlayers->at(i);
                p->Read();
                if (p->IsValid())
                    Players->push_back(p);
            }
        }

        // Updates //
        ESP->Update();
        AimAssist->Update();
        Trigger->Update();

        return true;
    } catch(const std::exception& ex) {
        //std::cout << "Error: " << ex.what() << std::endl;
        return true;
    }

    return false;
}

// Main
int main(int argc, char *argv[]) {
    if (getuid()) { 
        std::cout << "Please run as sudo!" << std::endl;
        return -1; 
    }

    // Waiting for Apex Legends to be found //
    while (Memory::GetPID() == 0) {
        std::system("clear");
        std::cout << "waiting for apex..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    std::system("clear");
    std::cout << "xap client ver 1.0" << std::endl;

    // Initialize Overlay Window //
    if (!InitializeOverlayWindow()) return -1;

    // Theading //
    std::thread X11Thread(X11EventListener);
    X11Thread.detach();
    std::thread InputManagerThread(InputManager::run);
    InputManagerThread.detach();

    // Initialize the whole process //
    try {
        for (int i = 0; i < 70; i++) 
            HumanPlayers->push_back(new Player(i, Myself));

        for (int i = 0; i < 15000; i++) 
            Dummies->push_back(new Player(i, Myself));

        ESP->Setup();
        
        std::cout << "core initialized" << std::endl;
        std::cout << "-----------------------------" << std::endl;

        LoadConfig();
        // This is where the fun starts //
        OverlayWindow.Start(&UpdateCore, &RenderUI);
    }
    catch(...) {}

    StopThread = true;
    InputManager::StopThread = true;
    X11Thread.join();
    InputManagerThread.join();

    return 0;
}