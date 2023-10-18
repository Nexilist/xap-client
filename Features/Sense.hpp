#pragma once
#include <iostream>
#include <vector>
#include "../Core/Player.hpp"
#include "../Core/Offsets.hpp"
#include "../Core/GlowMode.hpp"
#include "../Utils/Memory.hpp"
#include "../Utils/Color.hpp"
#include "../Utils/Conversion.hpp"
#include "../Utils/Config.hpp"

// UI //
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

struct Sense {
    // Glow
    bool GlowEnabled = true;
    float GlowMaxDistance = 200;

    // Item Glow
    std::vector<GlowMode>* StoredGlowMode = new std::vector<GlowMode>;
    bool ItemGlow = true;

    // Health and Armor (LATER)

    std::vector<Player*>* Players;

    Sense(std::vector<Player*>* Players) {
        this->Players = Players;
    }

    void RenderUI() {
        if (ImGui::BeginTabItem("Sense", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton | ImGuiTabItemFlags_NoReorder)) {
            // Glow //
            ImGui::Checkbox("Glow##ESP", &GlowEnabled);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Toggle Glowing");
            ImGui::SameLine();
            ImGui::Checkbox("Item Glow", &ItemGlow);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("You will have Loba's ability to see rare items");
            ImGui::SliderFloat("Max Glow Distance", &GlowMaxDistance, 0, 1000, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Only those in range will glow");

            ImGui::EndTabItem();
        }
    }

    bool Save() {
        try {
            Config::Glow::Enabled = GlowEnabled;
            Config::Glow::ItemGlow = ItemGlow;
            Config::Glow::MaxDistance = GlowMaxDistance;
            return true;
        } catch (...) {
            return false;
        }
    }

    void Setup() {
        for (int placebo = 0; placebo < 31; placebo++) {
            const GlowMode Ehh = { 135, 132, 35, 127 };
            StoredGlowMode->push_back(Ehh);
        }

        const GlowMode FirstStyle = { 135, 135, 128, 64 };
        const GlowMode SecondStyle = { 135, 135, 160, 64 };
        const GlowMode ThirdStyle = { 135, 135, 255, 64 };
        const GlowMode FourthStyle = { 135, 135, 32, 64 };

        StoredGlowMode->push_back(FirstStyle);
        StoredGlowMode->push_back(SecondStyle);
        StoredGlowMode->push_back(ThirdStyle);
        StoredGlowMode->push_back(FourthStyle);
    }

    void SetGlowState(long HighlightSettingsPointer, long HighlightSize, int HighlightID, GlowMode NewGlowMode) {
        const GlowMode oldGlowMode = Memory::Read<GlowMode>(HighlightSettingsPointer + (HighlightSize * HighlightID) + 4);
        if (NewGlowMode != oldGlowMode)
            Memory::Write<GlowMode>(HighlightSettingsPointer + (HighlightSize * HighlightID) + 4, NewGlowMode);
    }

    void Update() {
        const long HighlightSettingsPointer = Memory::Read<long>(OFF_REGION + OFF_GLOW_HIGHLIGHTS);
        const long HighlightSize = 0x28;

        // Item Glow //
        if (ItemGlow) {
            for (int highlightId = 31; highlightId < 35; highlightId++) {
                const GlowMode newGlowMode = { 137,138,35,127 };
                SetGlowState(HighlightSettingsPointer, HighlightSize, highlightId, newGlowMode);
            }
        }
        else {
            for (int highlightId = 31; highlightId < 35; highlightId++) {
                const GlowMode newGlowMode = StoredGlowMode->at(highlightId);
                SetGlowState(HighlightSettingsPointer, HighlightSize, highlightId, newGlowMode);
            }
        }

        // Player Glow //
        const GlowMode VisibleMode = { 2, 6, 40, 127 };
        const GlowMode InvisibleMode = { 2, 108, 40, 100 };
        const GlowMode NoGlowMode = { 0, 0, 0, 0 };
        for (int i = 0; i < Players->size(); i++) {
            Player* p = Players->at(i);
            if (!p->IsValid()) continue;
            if (!p->IsHostile) continue;
            if (GlowEnabled) {
                if (p->DistanceToLocalPlayer > (Conversion::ToGameUnits(GlowMaxDistance))) continue;
                SetGlowState(HighlightSettingsPointer, HighlightSize, 0, VisibleMode);
                SetGlowState(HighlightSettingsPointer, HighlightSize, 1, InvisibleMode);
                p->EnableGlow();

            } else {
                SetGlowState(HighlightSettingsPointer, HighlightSize, 0, NoGlowMode);
                SetGlowState(HighlightSettingsPointer, HighlightSize, 1, NoGlowMode);
                p->DisableGlow();
            }
        }

        // Target Locked Glow //
        const GlowMode LockedGlowMode = { 109, 108, 40, 100 };
        const Color LockedOnColor = { 0, 5, 5 };
        SetGlowState(HighlightSettingsPointer, HighlightSize, 2, LockedGlowMode);
        const Color oldColor = Memory::Read<Color>(HighlightSettingsPointer + (HighlightSize * 2) + 8);
        if (oldColor != LockedOnColor)
            Memory::Write<Color>(HighlightSettingsPointer + (HighlightSize * 2) + 8, LockedOnColor);

    }
};