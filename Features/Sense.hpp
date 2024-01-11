#pragma once
#include "../Core/Camera.hpp"
#include "../Core/GlowMode.hpp"
#include "../Core/LocalPlayer.hpp"
#include "../Core/Offsets.hpp"
#include "../Core/Player.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "../Features/Aimbot.hpp"

#include "../Overlay/Overlay.hpp"
#include "../Overlay/Renderer.hpp"

#include "../Utils/Color.hpp"
#include "../Utils/Config.hpp"
#include "../Utils/Conversion.hpp"
#include "../Utils/HitboxType.hpp"
#include "../Utils/Memory.hpp"

// UI //
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

// Geometry
#ifndef DEG2RAD
#define DEG2RAD(x) ((float)(x) * (float)(M_PI / 180.f))
#endif

struct Sense {
  // Glow
  bool GlowEnabled = true;
  float GlowMaxDistance = 200;

  std::vector<GlowMode> *StoredGlowMode = new std::vector<GlowMode>;
  bool ItemGlow = true;

  // Drawings
  bool DrawBox = true;
  bool Skeleton = true;
  float SkeletonThickness = 1.5f;
  float ESPMaxDistance = 200;
  bool ShowNear = true;
  bool ShowSpectators = true;
  bool DrawSeer = true;
  bool DrawTracers = true;
  bool DrawDistance = true;
  bool DrawFOVCircle = true;
  bool DrawNames = true;
  bool Team = true;

  // Settings
  bool AimedAtOnly = false;
  float GameFOV = 120;

  // Variables
  Camera *GameCamera;
  LocalPlayer *Myself;
  std::vector<Player *> *Players;
  std::chrono::milliseconds LastUpdateTime;
  int TotalSpectators = 0;
  std::vector<std::string> Spectators;

  Sense(std::vector<Player *> *Players, Camera *GameCamera,
        LocalPlayer *Myself) {
    this->Players = Players;
    this->GameCamera = GameCamera;
    this->Myself = Myself;
  }

  void RenderUI() {
    if (ImGui::BeginTabItem("Sense", nullptr,
                            ImGuiTabItemFlags_NoCloseWithMiddleMouseButton |
                                ImGuiTabItemFlags_NoReorder)) {
      ImGui::Text("Team Toggle (LOAD CONFIG TO APPLY CHANGES)");
      ImGui::Checkbox("Team Gamemode?", &Team);
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Enable if playing Control/TDM, Disable if playing "
                          "BR/Gun Run/Training Area");

      ImGui::Separator();

      // Glow //
      ImGui::Text("Glow");
      ImGui::Checkbox("Glow##ESP", &GlowEnabled);
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Toggle Glowing");
      ImGui::SameLine();
      ImGui::Checkbox("Item Glow", &ItemGlow);
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("You will have Loba's ability to see weapons");
      ImGui::SliderFloat("Glow Max Distance", &GlowMaxDistance, 0, 1000,
                         "%.0f");
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Only those in range will glow");

      ImGui::Separator();

      // Drawings
      ImGui::Text("ESP");
      ImGui::Checkbox("Draw Box", &DrawBox);
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Drawbox on enemy");

      ImGui::Checkbox("Draw Tracers", &DrawTracers);
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Draw lines to enemies");

      ImGui::Checkbox("Draw Skeleton", &Skeleton);
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Draw the enemies skeletons (Spooky)");

      ImGui::Checkbox("Draw Names", &DrawNames);
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Show enemies names");

      ImGui::Checkbox("Draw Distance", &DrawDistance);
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Show how far away the enemies are");

      ImGui::Checkbox("Draw Seer", &DrawSeer);
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip(
            "Draw Seer's abilitiy (Show Health & Armor) on enemies");

      ImGui::Checkbox("Show Enemy Count Near", &ShowNear);
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Show how many enemies are near");

      ImGui::SliderFloat("ESP Max Distance", &ESPMaxDistance, 0, 1000, "%.0f");
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Only those in range will ESP");

      ImGui::Separator();
      ImGui::Text("FOV Settings");
      ImGui::Checkbox("Draw FOV Circle", &DrawFOVCircle);
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Draw FOV Circle");
      ImGui::SliderFloat("Game's FOV", &GameFOV, 70, 120, "%.0f");
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Your current FOV in Settings");

      ImGui::Separator();
      ImGui::Text("Misc");
      ImGui::Checkbox("Show Spectators", &ShowSpectators);
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Show spectators");

      // Sense Settings
      ImGui::Separator();
      ImGui::Text("Sense Settings");
      ImGui::Checkbox("Aimed At Only##ESP", &AimedAtOnly);
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Only draw those who are locked on by Aim-Assist");

      ImGui::EndTabItem();
    }
  }

  bool Save() {
    try {
      Config::Sense::Enabled = GlowEnabled;
      Config::Sense::ItemGlow = ItemGlow;
      Config::Sense::MaxDistance = GlowMaxDistance;
      Config::Sense::DrawSeer = DrawSeer;
      Config::Sense::AimedAtOnly = AimedAtOnly;
      Config::Sense::ShowSpectators = ShowSpectators;
      Config::Sense::DrawFOVCircle = DrawFOVCircle;
      Config::Sense::GameFOV = GameFOV;
      Config::Sense::DrawBox = DrawBox;
      Config::Sense::Skeleton = Skeleton;
      Config::Sense::ESPMaxDistance = ESPMaxDistance;
      Config::Sense::ShowNear = ShowNear;
      Config::Sense::DrawNames = DrawNames;
      Config::Sense::Team = Team;
      return true;
    } catch (...) {
      return false;
    }
  }

  void Initialize() {
    for (int placebo = 0; placebo < 31; placebo++) {
      const GlowMode Ehh = {135, 132, 35, 127};
      StoredGlowMode->push_back(Ehh);
    }

    const GlowMode FirstStyle = {135, 135, 128, 64};
    const GlowMode SecondStyle = {135, 135, 160, 64};
    const GlowMode ThirdStyle = {135, 135, 255, 64};
    const GlowMode FourthStyle = {135, 135, 32, 64};

    StoredGlowMode->push_back(FirstStyle);
    StoredGlowMode->push_back(SecondStyle);
    StoredGlowMode->push_back(ThirdStyle);
    StoredGlowMode->push_back(FourthStyle);
  }

  void RenderDrawings(ImDrawList *Canvas, Aimbot *AimAssistState,
                      LocalPlayer *Myself, Overlay OverlayWindow) {
    int ScreenWidth;
    int ScreenHeight;
    OverlayWindow.GetScreenResolution(ScreenWidth, ScreenHeight);

    if (ShowSpectators) {
      ImVec2 Center = ImGui::GetMainViewport()->GetCenter();
      ImGui::SetNextWindowPos(ImVec2(0.0f, Center.y), ImGuiCond_Once,
                              ImVec2(0.02f, 0.5f));
      ImGui::SetNextWindowBgAlpha(0.3f);
      ImGui::Begin(
          "Spectators", nullptr,
          ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar |
              ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove |
              ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoCollapse |
              ImGuiWindowFlags_NoScrollbar);

      std::chrono::milliseconds Now =
          std::chrono::duration_cast<std::chrono::milliseconds>(
              std::chrono::system_clock::now().time_since_epoch());
      if (Now >= LastUpdateTime + std::chrono::milliseconds(1500)) {
        int TempTotalSpectators = 0;
        std::vector<std::string> TempSpectators;

        for (int i = 0; i < Players->size(); i++) {
          Player *p = Players->at(i);
          if (p->BasePointer == Myself->BasePointer)
            continue;
          if (p->GetViewYaw() == Myself->ViewYaw && p->IsDead) {
            TempTotalSpectators++;
            TempSpectators.push_back(p->GetPlayerName());
          }
        }

        TotalSpectators = TempTotalSpectators;
        Spectators = TempSpectators;
        LastUpdateTime = Now;
      }
      ImGui::Text("Spectators: ");
      ImGui::SameLine();
      ImGui::TextColored(TotalSpectators > 0 ? ImVec4(1, 0.343, 0.475, 1)
                                             : ImVec4(0.4, 1, 0.343, 1),
                         "%d", TotalSpectators);
      if (static_cast<int>(Spectators.size()) > 0) {
        ImGui::Separator();
        for (int i = 0; i < static_cast<int>(Spectators.size()); i++) {
          ImGui::TextColored(ImVec4(1, 0.343, 0.475, 1), "> %s",
                             Spectators.at(i).c_str());
        }
      }
      ImGui::End();
    }

    // Draw FOV Circle
    if (DrawFOVCircle && Myself->IsCombatReady()) {
      float FOV =
          std::min(AimAssistState->FOV,
                   AimAssistState->FOV * (AimAssistState->GetFOVScale() *
                                          AimAssistState->ZoomScale));
      float Radius =
          tanf(DEG2RAD(FOV) / 2) / tanf(DEG2RAD(GameFOV) / 2) * ScreenWidth;
      Renderer::DrawCircle(Canvas, Vector2D(ScreenWidth / 2, ScreenHeight / 2),
                           Radius, 40, ImColor(255, 255, 255), 2);
    }

    // Draw lot of things
    int PlayersNear = 0;
    for (int i = 0; i < Players->size(); i++) {
      Player *p = Players->at(i);
      if (p->IsCombatReady() && p->IsHostile &&
          p->DistanceToLocalPlayer <
              (Conversion::ToGameUnits(ESPMaxDistance))) {
        PlayersNear++;
        // Tracers
        if (DrawTracers) {
          Vector2D chestScreenPosition;
          GameCamera->WorldToScreen(p->LocalOrigin.Add(Vector3D(0, 0, 0)),
                                    chestScreenPosition);
          if (!chestScreenPosition.IsZeroVector()) {
            int x = (int)(ScreenWidth * 0.5f);

            ImColor TracerColor;
            TracerColor = ImColor(255, 255, 255);

            if (p->IsVisible) {
              TracerColor = ImColor(0, 255, 0);
            } else if (!p->IsVisible) {
              TracerColor = ImColor(255, 0, 0);
            }

            Renderer::DrawLine(Canvas, Vector2D(x, ScreenHeight),
                               chestScreenPosition, 1.5f, ImColor(TracerColor));
            Renderer::DrawCircleFilled(Canvas, chestScreenPosition, 2, 10,
                                       ImColor(TracerColor));
          }
        }

        // Distance
        if (DrawDistance) {
          Vector2D originScreenPosition;
          GameCamera->WorldToScreen(p->LocalOrigin.Add(Vector3D(0, 0, -4)),
                                    originScreenPosition);
          if (!originScreenPosition.IsZeroVector()) {
            Renderer::DrawText(Canvas, originScreenPosition.Add(Vector2D(0, 0)),
                               std::to_string((int)Conversion::ToMeters(
                                                  p->DistanceToLocalPlayer))
                                   .c_str(),
                               ImColor(255, 255, 255), true, true, false);
          }
        }

        // DrawBox
        if (DrawBox && p->DistanceToLocalPlayer <
                           (Conversion::ToGameUnits(ESPMaxDistance / 2))) {

          Vector2D Head, Foot;
          GameCamera->WorldToScreen(p->GetBonePosition(HitboxType::Head), Head);
          GameCamera->WorldToScreen(p->LocalOrigin.Add(Vector3D(0, 0, 0)),
                                    Foot);
          if (p->IsVisible) {
            Renderer::DrawBox(Canvas, Foot, Head, ImColor(0, 255, 0), 2);
          } else if (!p->IsVisible)
            Renderer::DrawBox(Canvas, Foot, Head, ImColor(255, 0, 0), 2);
        }

        // Draw Names
        if (DrawNames && !p->IsDummy()) {
          Vector2D originScreenPosition;
          GameCamera->WorldToScreen(p->LocalOrigin.Add(Vector3D(0, 0, 110)),
                                    originScreenPosition);
          Renderer::DrawText(Canvas, originScreenPosition.Add(Vector2D(0, 0)),
                             p->GetPlayerName().c_str(), ImVec4(1.0, 1, 1.0, 1),
                             true, true, false);
        }
        //}
        // Draw Skeleton
        if (Skeleton) {
          ImColor SkeletonColor;
          SkeletonColor = ImColor(255, 255, 255);

          Vector2D Head, Neck, UpperChest, LowerChest, Stomach, Leftshoulder,
              Leftelbow, LeftHand, Rightshoulder, RightelbowBone, RightHand,
              LeftThighs, Leftknees, Leftleg, RightThighs, Rightknees, Rightleg;
          GameCamera->WorldToScreen(p->GetBonePosition(HitboxType::Head), Head);
          GameCamera->WorldToScreen(p->GetBonePosition(HitboxType::Neck), Neck);
          GameCamera->WorldToScreen(p->GetBonePosition(HitboxType::UpperChest),
                                    UpperChest);
          GameCamera->WorldToScreen(p->GetBonePosition(HitboxType::LowerChest),
                                    LowerChest);
          GameCamera->WorldToScreen(p->GetBonePosition(HitboxType::Stomach),
                                    Stomach);

          GameCamera->WorldToScreen(
              p->GetBonePosition(HitboxType::Leftshoulder), Leftshoulder);
          GameCamera->WorldToScreen(p->GetBonePosition(HitboxType::Leftelbow),
                                    Leftelbow);
          GameCamera->WorldToScreen(p->GetBonePosition(HitboxType::LeftHand),
                                    LeftHand);

          GameCamera->WorldToScreen(
              p->GetBonePosition(HitboxType::Rightshoulder), Rightshoulder);
          GameCamera->WorldToScreen(
              p->GetBonePosition(HitboxType::RightelbowBone), RightelbowBone);
          GameCamera->WorldToScreen(p->GetBonePosition(HitboxType::RightHand),
                                    RightHand);

          GameCamera->WorldToScreen(p->GetBonePosition(HitboxType::LeftThighs),
                                    LeftThighs);
          GameCamera->WorldToScreen(p->GetBonePosition(HitboxType::Leftknees),
                                    Leftknees);
          GameCamera->WorldToScreen(p->GetBonePosition(HitboxType::Leftleg),
                                    Leftleg);

          GameCamera->WorldToScreen(p->GetBonePosition(HitboxType::RightThighs),
                                    RightThighs);
          GameCamera->WorldToScreen(p->GetBonePosition(HitboxType::Rightknees),
                                    Rightknees);
          GameCamera->WorldToScreen(p->GetBonePosition(HitboxType::Rightleg),
                                    Rightleg);

          // Renderer::DrawCircle(Canvas, Head, 10.0f, 10, SkeletonColor,
          // SkeletonThickness); Renderer::DrawCircleFilled(Canvas, Head, 2, 10,
          // ImColor(255, 255, 255));

          Renderer::DrawLine(Canvas, Head, Neck, SkeletonThickness,
                             SkeletonColor);
          Renderer::DrawLine(Canvas, Neck, UpperChest, SkeletonThickness,
                             SkeletonColor);
          Renderer::DrawLine(Canvas, UpperChest, LowerChest, SkeletonThickness,
                             SkeletonColor);
          Renderer::DrawLine(Canvas, LowerChest, Stomach, SkeletonThickness,
                             SkeletonColor);

          Renderer::DrawLine(Canvas, Neck, Leftshoulder, SkeletonThickness,
                             SkeletonColor);
          Renderer::DrawLine(Canvas, Leftshoulder, Leftelbow, SkeletonThickness,
                             SkeletonColor);
          Renderer::DrawLine(Canvas, Leftelbow, LeftHand, SkeletonThickness,
                             SkeletonColor);

          Renderer::DrawLine(Canvas, Neck, Rightshoulder, SkeletonThickness,
                             SkeletonColor);
          Renderer::DrawLine(Canvas, Rightshoulder, RightelbowBone,
                             SkeletonThickness, SkeletonColor);
          Renderer::DrawLine(Canvas, RightelbowBone, RightHand,
                             SkeletonThickness, SkeletonColor);

          Renderer::DrawLine(Canvas, Stomach, LeftThighs, SkeletonThickness,
                             SkeletonColor);
          Renderer::DrawLine(Canvas, LeftThighs, Leftknees, SkeletonThickness,
                             SkeletonColor);
          Renderer::DrawLine(Canvas, Leftknees, Leftleg, SkeletonThickness,
                             SkeletonColor);

          Renderer::DrawLine(Canvas, Stomach, RightThighs, SkeletonThickness,
                             SkeletonColor);
          Renderer::DrawLine(Canvas, RightThighs, Rightknees, SkeletonThickness,
                             SkeletonColor);
          Renderer::DrawLine(Canvas, Rightknees, Rightleg, SkeletonThickness,
                             SkeletonColor);
        }
      }

      if (!p->IsCombatReady() || !p->IsVisible || !p->IsHostile ||
          p->DistanceToLocalPlayer >
              (Conversion::ToGameUnits(ESPMaxDistance)) ||
          Myself->BasePointer == p->BasePointer)
        continue;
      // Tracer OLD HIRE

      // Seer
      if (DrawSeer && !AimedAtOnly) {
        Vector2D headScreenPosition;
        GameCamera->WorldToScreen(p->GetBonePosition(HitboxType::Head),
                                  headScreenPosition);
        if (!headScreenPosition.IsZeroVector())
          Renderer::DrawSeer(Canvas, headScreenPosition.x,
                             headScreenPosition.y - 20, p->Shield, p->MaxShield,
                             p->Health);
      }
    }

    if (ShowNear) {
      // Gui DrawText Version
      Renderer::DrawText(Canvas,
                         Vector2D(ScreenWidth * 0.5, ScreenHeight * 0.6),
                         ("NEAR : " + std::to_string(PlayersNear)).c_str(),
                         ImVec4(1.0f, 1.0f, 1.0f, 1.0f), true, true, false);
      // Gui Version
      /*
     ImVec2 Center = ImGui::GetMainViewport()->GetCenter();
     ImGui::SetNextWindowPos(ImVec2(Center.x, Center.y * 1.2), ImGuiCond_Once,
     ImVec2(0.50f, 0.5f)); ImGui::SetNextWindowBgAlpha(0.3f);
     ImGui::Begin("Near", nullptr, ImGuiWindowFlags_AlwaysAutoResize |
     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings |
     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs |
     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
     ImGui::Text("Near: ");
     ImGui::SameLine();
     ImGui::TextColored(PlayersNear > 0 ? ImVec4(0.4, 1, 0.343, 1) : ImVec4(1,
     1, 1, 1), "%d", PlayersNear); ImGui::End();
     */
    }
    PlayersNear = 0;
    // Draw Seer on locked target
    if (AimAssistState->TargetSelected && AimAssistState->CurrentTarget) {
      Vector2D headScreenPosition;
      GameCamera->WorldToScreen(
          AimAssistState->CurrentTarget->GetBonePosition(HitboxType::Head),
          headScreenPosition);
      if (headScreenPosition.IsZeroVector())
        return;

      Renderer::DrawSeer(Canvas, headScreenPosition.x,
                         headScreenPosition.y - 20,
                         AimAssistState->CurrentTarget->Shield,
                         AimAssistState->CurrentTarget->MaxShield,
                         AimAssistState->CurrentTarget->Health);
      return;
    }
  }

  void SetGlowState(long HighlightSettingsPointer, long HighlightSize,
                    int HighlightID, GlowMode NewGlowMode) {
    const GlowMode oldGlowMode = Memory::Read<GlowMode>(
        HighlightSettingsPointer + (HighlightSize * HighlightID) + 0x0);
    if (NewGlowMode != oldGlowMode)
      Memory::Write<GlowMode>(HighlightSettingsPointer +
                                  (HighlightSize * HighlightID) + 0x0,
                              NewGlowMode);
  }

  void SetColorState(long HighlightSettingsPointer, long HighlightSize,
                     int HighlightID, Color NewColor) {
    const Color oldColor = Memory::Read<Color>(
        HighlightSettingsPointer + (HighlightSize * HighlightID) + 0x4);
    if (oldColor != NewColor)
      Memory::Write<Color>(HighlightSettingsPointer +
                               (HighlightSize * HighlightID) + 0x4,
                           NewColor);
  }

  void SetGlow(Player *Target, int GlowEnabled, int GlowThroughWall,
               int HighlightID) {
    if (Target->GlowEnable != GlowEnabled)
      Memory::Write<int>(Target->BasePointer + OFF_GLOW_ENABLE, GlowEnabled);
    if (Target->GlowThroughWall != GlowThroughWall) {
      Memory::Write<int>(Target->BasePointer + OFF_GLOW_THROUGH_WALL,
                         GlowThroughWall);
      Memory::Write<int>(Target->BasePointer + OFF_GLOW_FIX, 2);
    }
    if (Target->HighlightID != HighlightID)
      Memory::Write<int>(Target->BasePointer + OFF_GLOW_HIGHLIGHT_ID + 1,
                         HighlightID);
  }

  void Update() {
    const long HighlightSettingsPointer =
        Memory::Read<long>(OFF_REGION + OFF_GLOW_HIGHLIGHTS);
    const long HighlightSize = 0x34;

    // Item Glow //
    if (ItemGlow) {
      for (int highlightId = 30; highlightId < 40; highlightId++) {
        const GlowMode newGlowMode = {137, 0, 0, 127};
        SetGlowState(HighlightSettingsPointer, HighlightSize, highlightId,
                     newGlowMode);
      }
    } else {
      for (std::vector<GlowMode>::size_type highlightId = 30;
           highlightId <
           std::min(static_cast<std::vector<GlowMode>::size_type>(40),
                    StoredGlowMode->size());
           ++highlightId) {
        const GlowMode newGlowMode = StoredGlowMode->at(highlightId);
        SetGlowState(HighlightSettingsPointer, HighlightSize, highlightId,
                     newGlowMode);
      }
    }

    // Player Glow //
    // -> Visible
    const GlowMode VisibleMode = {2, 4, 20, 127};
    const Color VisibleColor = {0, 255, 0};
    SetGlowState(HighlightSettingsPointer, HighlightSize, 0, VisibleMode);
    SetColorState(HighlightSettingsPointer, HighlightSize, 0, VisibleColor);

    // -> Invisible
    const GlowMode InvisibleMode = {2, 4, 20, 127};
    const Color InvisibleColor = {255, 0, 0};
    SetGlowState(HighlightSettingsPointer, HighlightSize, 1, InvisibleMode);
    SetColorState(HighlightSettingsPointer, HighlightSize, 1, InvisibleColor);

    // -> Knocked
    const GlowMode KnockedMode = {2, 4, 20, 127};
    const Color KnockedColor = {255, 150, 0};
    SetGlowState(HighlightSettingsPointer, HighlightSize, 90, KnockedMode);
    SetColorState(HighlightSettingsPointer, HighlightSize, 90, KnockedColor);

    // -> Disabled
    const GlowMode DisabledMode = {0, 0, 0, 0};
    const Color DisabledColor = {1, 1, 1};
    SetGlowState(HighlightSettingsPointer, HighlightSize, 91, DisabledMode);
    SetColorState(HighlightSettingsPointer, HighlightSize, 91, DisabledColor);

    // -> Locked On
    const GlowMode LockedOnMode = {136, 6, 32, 127};
    const Color LockedOnColor = {0, 0.75, 0.75};
    SetGlowState(HighlightSettingsPointer, HighlightSize, 92, LockedOnMode);
    SetColorState(HighlightSettingsPointer, HighlightSize, 92, LockedOnColor);

    for (int i = 0; i < Players->size(); i++) {
      Player *Target = Players->at(i);
      if (!Target->IsValid())
        continue;
      if (!Target->IsHostile)
        continue;

      if (GlowEnabled) {
        if (Target->IsLockedOn) {
          SetGlow(Target, 1, 2, 92);
          continue;
        }

        if (Target->DistanceToLocalPlayer <
            Conversion::ToGameUnits(GlowMaxDistance)) {
          if (Target->IsKnocked) {
            SetGlow(Target, 1, 2, 90);
            continue;
          }

          int Highlight = (Target->IsVisible) ? 0 : 1;
          SetGlow(Target, 1, 2, Highlight);
          continue;
        }
      }

      SetGlow(Target, 0, 0, 91);
    }
  }
};
