#pragma once
#include <iostream>
#include <vector>
#include "../Core/Player.hpp"
#include "../Core/LocalPlayer.hpp"
#include "../Core/Offsets.hpp"

#include "../Math/Vector2D.hpp"
#include "../Math/Vector3D.hpp"
#include "../Math/QAngle.hpp"
#include "../Math/Resolver.hpp"

#include "../Utils/Memory.hpp"
#include "../Utils/XDisplay.hpp"
#include "../Utils/Conversion.hpp"
#include "../Utils/Config.hpp"
#include "../Utils/HitboxType.hpp"

// UI //
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

struct Aimbot {
    bool AimbotEnabled = true;
    bool AllowTargetSwitch = true;

    bool PredictMovement = true;
    bool PredictBulletDrop = true;

    HitboxType Hitbox = HitboxType::UpperChest;

    float FinalDistance = 0;
    float FinalFOV = 0;

    float Speed = 40;
    float Smooth = 10;
    float ExtraSmooth = 250;
    float HipfireFOV = 30;
    float ZoomFOV = 10;
    float MinDistance = 1;
    float HipfireDistance = 60;
    float ZoomDistance = 160;

    XDisplay* X11Display;
    LocalPlayer* Myself;
    std::vector<Player*>* Players;

    Player* CurrentTarget = nullptr;
    bool TargetSelected = true;

    Aimbot(XDisplay* X11Display, LocalPlayer* Myself, std::vector<Player*>* GamePlayers) {
        this->X11Display = X11Display;
        this->Myself = Myself;
        this->Players = GamePlayers;
    }

    void RenderUI() {
        if (ImGui::BeginTabItem("Aim", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton | ImGuiTabItemFlags_NoReorder)) {
            ImGui::Checkbox("Aim - Assist", &AimbotEnabled);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Toggle the Aim-Assist");
            ImGui::SameLine();
            ImGui::Checkbox("Allow Target Switch", &AllowTargetSwitch);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Allow switching Target when current target no longer meets requirements");

            ImGui::Separator();

            ImGui::Checkbox("Predict Movement", &PredictMovement);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Predict target's movement");
            ImGui::SameLine();
            ImGui::Checkbox("Predict Bullet Drop", &PredictBulletDrop);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Predict weapon's bullet drop");

            ImGui::Separator();

            const char* HitboxTypes[] = { "Head", "Neck", "Upper Chest", "Lower Chest", "Stomach", "Hip" };
            int HitboxTypeIndex = static_cast<int>(Hitbox);
            ImGui::Combo("Hitbox Type", &HitboxTypeIndex, HitboxTypes, IM_ARRAYSIZE(HitboxTypes));
            Hitbox = static_cast<HitboxType>(HitboxTypeIndex);

            ImGui::Separator();

            ImGui::SliderFloat("Speed", &Speed, 1, 100, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Speed of the Aim-Assist\nBigger = Faster");

            ImGui::Separator();

            ImGui::SliderFloat("Smooth", &Smooth, 1, 20, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Smoothness for the Aim-Assist\nSmaller = Faster and vice versa");
            ImGui::SliderFloat("Extra Smooth", &ExtraSmooth, 100, 5000, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Extra smoothness by calculating the distance between you and the target");

            ImGui::Separator();

            ImGui::SliderFloat("Hipfire FOV", &HipfireFOV, 1, 180, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Field of View");
            ImGui::SliderFloat("Zoom FOV", &ZoomFOV, 1, 180, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Field of View");

            ImGui::Separator();

            ImGui::SliderFloat("Hip-fire Distance", &HipfireDistance, 1, 500, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Minimum distance for Aim-Assist to work");
            ImGui::SliderFloat("Zoom Distance", &ZoomDistance, 1, 500, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Maximum distance for Aim-Assist to work");

            ImGui::EndTabItem();
        }
    }

    bool Save() {
        try {
            Config::Aimbot::Enabled = AimbotEnabled;
            Config::Aimbot::AllowTargetSwitch = AllowTargetSwitch;
            Config::Aimbot::PredictMovement = PredictMovement;
            Config::Aimbot::PredictBulletDrop = PredictBulletDrop;
            Config::Aimbot::Hitbox = static_cast<int>(Hitbox);
            Config::Aimbot::Speed = Speed;
            Config::Aimbot::Smooth = Smooth;
            Config::Aimbot::ExtraSmooth = ExtraSmooth;
            Config::Aimbot::HipfireFOV = HipfireFOV;
            Config::Aimbot::ZoomFOV = ZoomFOV;
            Config::Aimbot::MinDistance = MinDistance;
            Config::Aimbot::HipfireDistance = HipfireDistance;
            Config::Aimbot::ZoomDistance = ZoomDistance;
            return true;
        } catch (...) {
            return false;
        }
    }

    void Update() {
        if (!AimbotEnabled) { ReleaseTarget(); return; }

        if (Myself->IsZooming) {
            FinalFOV = ZoomFOV;
            FinalDistance = ZoomDistance;
        }
        else {
            FinalFOV = HipfireFOV;
            FinalDistance = HipfireDistance;
        }

        if (!Myself->IsCombatReady()) { TargetSelected = false; return; }
        if (!X11Display->KeyDown(XK_Shift_L) && !Myself->IsInAttack) { ReleaseTarget(); return; }

        Player* Target = CurrentTarget;
        if (!IsValidTarget(Target)) {
            if(TargetSelected && !AllowTargetSwitch)
                return;

            Target = FindBestTarget();
            if (!IsValidTarget(Target)) {
                CurrentTarget = nullptr;
                return;
            }
            
            CurrentTarget = Target;
            CurrentTarget->IsLockedOn = true;
            TargetSelected = true;
        }

        // Where the fun begins //
        double DistanceFromCrosshair = CalculateDistanceFromCrosshair(CurrentTarget);
        if (DistanceFromCrosshair > FinalFOV || DistanceFromCrosshair == -1) {
            ReleaseTarget();
            return;
        }
        StartAiming();
    }

    void StartAiming() {
        // Get Target Angle
        QAngle DesiredAngles = QAngle(0, 0);
        if (!GetAngle(CurrentTarget, DesiredAngles))
            return;

        // Calculate Increment
        Vector2D DesiredAnglesIncrement = Vector2D(CalculatePitchIncrement(DesiredAngles), CalculateYawIncrement(DesiredAngles));

        // Calculate Smooth
        float Extra = ExtraSmooth / CurrentTarget->DistanceToLocalPlayer;
        float TotalSmooth = Smooth + Extra;

        // No recoil calcs
        Vector2D punchAnglesDiff = Myself->PunchAnglesDifferent.Divide(Smooth).Multiply(Speed);
        double nrPitchIncrement = punchAnglesDiff.x;
        double nrYawIncrement = -punchAnglesDiff.y;

        // Aimbot calcs
        Vector2D aimbotDelta = DesiredAnglesIncrement.Divide(TotalSmooth).Multiply(Speed);
        double aimYawIncrement = aimbotDelta.y * -1;
        double aimPitchIncrement = aimbotDelta.x;

        // Combine
        double totalPitchIncrement = aimPitchIncrement + nrPitchIncrement;
        double totalYawIncrement = aimYawIncrement + nrYawIncrement;

        // Turn into integers
        int totalPitchIncrementInt = RoundHalfEven(AL1AF0(totalPitchIncrement));
        int totalYawIncrementInt = RoundHalfEven(AL1AF0(totalYawIncrement));

        // Move Mouse
        if (totalPitchIncrementInt == 0 && totalYawIncrementInt == 0) return;
        X11Display->MoveMouse(totalPitchIncrementInt, totalYawIncrementInt);
    }

    bool GetAngle(const Player* Target, QAngle& Angle) {
        const QAngle CurrentAngle = QAngle(Myself->ViewAngles.x, Myself->ViewAngles.y).fixAngle();
        if (!CurrentAngle.isValid())
            return false;

        if (!GetAngleToTarget(Target, Angle))
            return false;

        return true;
    }

    bool GetAngleToTarget(const Player* Target, QAngle& Angle) const {
        const Vector3D TargetPosition = Target->GetBonePosition(Hitbox);
        const Vector3D TargetVelocity = Target->AbsoluteVelocity;
        const Vector3D CameraPosition = Myself->CameraPosition;
        const QAngle CurrentAngle = QAngle(Myself->ViewAngles.x, Myself->ViewAngles.y).fixAngle();
        
        if (Myself->WeaponProjectileSpeed > 1.0f) {
            if (PredictBulletDrop && PredictMovement) {
                return Resolver::CalculateAimRotationNew(CameraPosition, TargetPosition, TargetVelocity, Myself->WeaponProjectileSpeed, Myself->WeaponProjectileScale, 255, Angle);
            }
            else if (PredictBulletDrop) {
                return Resolver::CalculateAimRotationNew(CameraPosition, TargetPosition, Vector3D(0, 0, 0), Myself->WeaponProjectileSpeed, Myself->WeaponProjectileScale, 255, Angle);
            }
            else if (PredictMovement) {
                return Resolver::CalculateAimRotation(CameraPosition, TargetPosition, TargetVelocity, Myself->WeaponProjectileSpeed, Angle);
            }
        }

        Angle = Resolver::CalculateAngle(CameraPosition, TargetPosition);
        return true;   
    }

    bool IsValidTarget(Player* target) {
        if (target == nullptr ||
            !target->IsValid() || 
            !target->IsCombatReady() || 
            !target->IsVisible || 
            !target->IsHostile || 
            target->Distance2DToLocalPlayer < Conversion::ToGameUnits(MinDistance) || 
            target->Distance2DToLocalPlayer > Conversion::ToGameUnits(FinalDistance))
            return false;
        return true;
    }

    float CalculatePitchIncrement(QAngle AimbotDesiredAngles) {
        float wayA = AimbotDesiredAngles.x - Myself->ViewAngles.x;
        float wayB = 180 - abs(wayA);
        if (wayA > 0 && wayB > 0)
            wayB *= -1;
        if (fabs(wayA) < fabs(wayB))
            return wayA;
        return wayB;
    }

    float CalculateYawIncrement(QAngle AimbotDesiredAngles) {
        float wayA = AimbotDesiredAngles.y - Myself->ViewAngles.y;
        float wayB = 360 - abs(wayA);
        if (wayA > 0 && wayB > 0)
            wayB *= -1;
        if (fabs(wayA) < fabs(wayB))
            return wayA;
        return wayB;
    }

    double CalculateDistanceFromCrosshair(Player* target) {
        Vector3D CameraPosition = Myself->CameraPosition;
        QAngle CurrentAngle = QAngle(Myself->ViewAngles.x, Myself->ViewAngles.y).fixAngle();

        Vector3D TargetPos = target->LocalOrigin;
        if (CameraPosition.Distance(TargetPos) <= 0.0001f)
            return -1;

        QAngle TargetAngle = Resolver::CalculateAngle(CameraPosition, TargetPos);
        if (!TargetAngle.isValid())
            return -1;
        
        return CurrentAngle.distanceTo(TargetAngle);
    }

    Player* FindBestTarget() {
        float NearestDistance = 9999;
        Player* BestTarget = nullptr;
        Vector3D CameraPosition = Myself->CameraPosition;
        QAngle CurrentAngle = QAngle(Myself->ViewAngles.x, Myself->ViewAngles.y).fixAngle();
        for (int i = 0; i < Players->size(); i++) {
            Player* p = Players->at(i);
            if (!IsValidTarget(p)) continue;

            double DistanceFromCrosshair = CalculateDistanceFromCrosshair(p);
            if (DistanceFromCrosshair > FinalFOV || DistanceFromCrosshair == -1)
                continue;

            if (DistanceFromCrosshair < NearestDistance) {
                BestTarget = p;
                NearestDistance = DistanceFromCrosshair;
            }
        }
        return BestTarget;
    }

    void ReleaseTarget() {
        if (CurrentTarget != nullptr && CurrentTarget->IsValid())
            CurrentTarget->IsLockedOn = false;
        
        TargetSelected = false;
        CurrentTarget = nullptr;
    }

    int RoundHalfEven(float x) {
        return (x >= 0.0)
            ? static_cast<int>(std::round(x))
            : static_cast<int>(std::round(-x)) * -1;
    }

    float AL1AF0(float num) {
        if (num > 0) return std::max(num, 1.0f);
        return std::min(num, -1.0f);
    }
};