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
#include "Core/Camera.hpp"

#include "Features/Aimbot.hpp"
#include "Features/Sense.hpp"
#include "Features/Triggerbot.hpp"

#include "Overlay/Overlay.hpp"

#include "Utils/Config.hpp"
#include "Utils/Memory.hpp"
#include "Utils/XDisplay.hpp"
#include "Extra.hpp"

// Objects
XDisplay* X11Display = new XDisplay();
Overlay OverlayWindow = Overlay();
ImDrawList* Canvas;

// Game Objects
Level* Map = new Level();
LocalPlayer* Myself = new LocalPlayer();
Camera* GameCamera = new Camera();

// Players
std::vector<Player*>* HumanPlayers = new std::vector<Player*>;
std::vector<Player*>* Dummies = new std::vector<Player*>;
std::vector<Player*>* Players = new std::vector<Player*>;

// Features
Sense* ESP = new Sense(Players, GameCamera);
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
    XEvent event;
    Display* display = XOpenDisplay(nullptr);
    unsigned int keycode = XKeysymToKeycode(display, XK_Insert);
    unsigned int modifiers = ControlMask;
    Window root = DefaultRootWindow(display);
    Bool owner_events = False;

    XGrabKey(display, keycode, modifiers, root, owner_events, GrabModeAsync, GrabModeAsync);
    std::cout << "XGrabKey done\n";
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
    GameCamera->Initialize(ScreenWidth, ScreenHeight);
    std::cout << "1. overlay initialized" << std::endl;
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
    AimAssist->PredictMovement = Config::Aimbot::PredictMovement;
    AimAssist->PredictBulletDrop = Config::Aimbot::PredictBulletDrop;
    AimAssist->Speed = Config::Aimbot::Speed;
    AimAssist->Smooth = Config::Aimbot::Smooth;
    AimAssist->ExtraSmooth = Config::Aimbot::ExtraSmooth;
    AimAssist->FOV = Config::Aimbot::FOV;
    AimAssist->ZoomScale = Config::Aimbot::ZoomScale;
    AimAssist->MinDistance = Config::Aimbot::MinDistance;
    AimAssist->HipfireDistance = Config::Aimbot::HipfireDistance;
    AimAssist->ZoomDistance = Config::Aimbot::ZoomDistance;
    AimAssist->RecoilEnabled = Config::Aimbot::RecoilControl;
    AimAssist->PitchPower = Config::Aimbot::PitchPower;
    AimAssist->YawPower = Config::Aimbot::YawPower;

    // ESP //
    ESP->GlowEnabled = Config::Glow::Enabled;
    ESP->ItemGlow = Config::Glow::ItemGlow;
    ESP->GlowMaxDistance = Config::Glow::MaxDistance;
    ESP->DrawSeer = Config::Glow::DrawSeer;
    ESP->SeerMaxDistance = Config::Glow::SeerMaxDistance;
    ESP->VisibleOnly = Config::Glow::VisibleOnly;
    ESP->DrawFOVCircle = Config::Glow::DrawFOVCircle;
    ESP->GameFOV = Config::Glow::GameFOV;

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
bool skin_state = false;
// Interface
ImVec4 ProcessingTimeColor;
void RenderUI() {
    auto io = ImGui::GetIO();
	ImGui::SetNextWindowSize(io.DisplaySize);
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("##Overlay", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoBackground
	);
	Canvas = ImGui::GetWindowDrawList();
    if (Map->IsPlayable)
        ESP->RenderDrawings(Canvas, AimAssist, Myself, OverlayWindow);
	ImGui::End();

    if (!IsMenuOpened) return;

    // Menu
    ImGui::SetNextWindowSizeConstraints(ImVec2(440, 420), ImVec2(440, 420));
    ImGui::SetNextWindowSize(ImVec2(440, 420), ImGuiCond_FirstUseEver);
    ImGui::Begin(" ", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
    
    ProcessingTimeColor = OverlayWindow.ProcessingTime > 20 ? ProcessingTimeColor = ImVec4(1, 0.343, 0.475, 1) : ProcessingTimeColor = ImVec4(0.4, 1, 0.343, 1);
    ImGui::TextColored(ProcessingTimeColor, "Processing Time: %02dms", OverlayWindow.ProcessingTime);
    ImGui::SameLine();

    if (OverlayWindow.AlignedButton("Unload chair", 1.0f, false)){
        OverlayWindow.DestroyOverlay();
        ImGui::DestroyContext();
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
            ImGui::TextColored(ImVec4(1, 0.2, 0.3, 1), "Changed and overworked by morigan");
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Extras", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton | ImGuiTabItemFlags_NoReorder)){
            ImGui::Checkbox("Skinchanger", &skin_state);
            if (skin_state == True){
                SkinChange();
            }
            ImGui::EndTabItem();
        }

        if(ImGui::BeginTabItem("Configs", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton | ImGuiTabItemFlags_NoReorder)){
            
            ImGui::Separator();
            if (OverlayWindow.AlignedButton("Load Config", 0.0f, true)){
                LoadConfig();
            }
            if (OverlayWindow.AlignedButton("Save Config", 0.0f, true)) {
                SaveConfig();
                std::cout << "config saved" << std::endl;
            }
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
        GameCamera->Update();
        ESP->Update();
        AimAssist->Update();
        Trigger->Update();

        return true;
    } catch(const std::exception& ex) {
        std::cout << "Error: " << ex.what() << std::endl << "UpdateCore function";
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
    std::cout << "xap client ver 1.2" << std::endl;

    // Initialize Overlay Window //
    if (!InitializeOverlayWindow()) return -1;

    // Theading //

    //start first thread and detach it to run on its own
    std::thread X11Thread(X11EventListener);
    X11Thread.detach();
    //start second thread and detach it to run on its own
    std::thread InputManagerThread(InputManager::run);
    InputManagerThread.detach();

    // Initialize the whole process //
    try {
        for (int i = 0; i < 70; i++){
            HumanPlayers->push_back(new Player(i, Myself));
        }
        for (int i = 0; i < 15000; i++){
            Dummies->push_back(new Player(i, Myself));
        }

        std::cout << "3. Esp Initialization      (line. 247)\n";
        ESP->Initialize();
        std::cout << "4. Esp Initialization DONE (line. 248)\n";
        
        std::cout << "core initialized" << std::endl;
        std::cout << "-----------------------------" << std::endl;
        
        // This is where the fun starts //
        LoadConfig();
        std::cout << "5. OverlayWindow.Start\n";
        OverlayWindow.Start(&UpdateCore, &RenderUI);
    }
    catch(...) {}

    StopThread = true;
    InputManager::StopThread = true;
    X11Thread.join();
    InputManagerThread.join();

    return 0;
}