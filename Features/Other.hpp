#include <iostream>
#include <vector>
#include "../Core/Player.hpp"
#include "../Core/LocalPlayer.hpp"
#include "../Core/Offsets.hpp"
#include "../Core/Camera.hpp"

#include "../Overlay/Renderer.hpp"

#include "../Utils/Memory.hpp"
#include "../Utils/Config.hpp"

// UI //
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
struct Other{
   // Spectator list
    bool spectator = true;
    bool BhopEnabled = true;

    int SpectatorCountTeam = 0;
    int SpectatorCount = 0;

    std::vector<Player*>* Players;
    LocalPlayer* localPlayer;

    Other(std::vector<Player*>* Players, LocalPlayer* localPlayer) {
        this->Players = Players;
        this->localPlayer = localPlayer;
    }

    void RenderUI() {
        if (ImGui::BeginTabItem("Other", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton | ImGuiTabItemFlags_NoReorder)) {
            
            ImGui::Checkbox("SpectatorCount", &spectator);

            ImGui::EndTabItem();
        }
    }

    bool Save() {
        try {
            Config::Other::spectator = spectator;
            return true;
        } catch (...) {
            return false;
        }
    }

    void Update(){
        Spectators();
    }

    int TeamPrev = 100, Prev = 100;

    Vector2D SpectatorPos;
    
    void Spectators(){
        //Is func enabled
        if (!spectator) return;

        for (int i = 0; i < Players->size(); i ++){
            Player *p = Players->at(i);

            float localYaw = localPlayer->yaw;
            float targetYaw = p->yaw;

            if (targetYaw < 360) targetYaw += 360;
            targetYaw += 90; // yaw is off by 90
            if (targetYaw > 360) targetYaw -= 360;

            if (localYaw < 360) localYaw += 360;
            localYaw += 90; // yaw is off by 90
            if (localYaw > 360) localYaw -= 360;

            if ((targetYaw == localYaw) && p->Health == 0){
                if (p->Team != localPlayer->Team) SpectatorCount ++;
                else SpectatorCountTeam ++;
            }
        }
    }


};