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

// Conversion
#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI / 180.f) )

struct Aimbot {
    bool AimbotEnabled = true;

    bool PredictMovement = true;
    bool PredictBulletDrop = true;

    float FinalDistance = 0;

    float Speed = 40;
    float Smooth = 10;
    float FOV = 10;
    float ZoomScale = 1.2;
    float MinDistance = 1;
    float HipfireDistance = 60;
    float ZoomDistance = 160;

    bool RecoilEnabled = true;
    float PitchPower = 3;
    float YawPower = 3;

    XDisplay* X11Display;
    LocalPlayer* Myself;
    std::vector<Player*>* Players;

    Player* CurrentTarget = nullptr;
    bool TargetSelected = true;
    QAngle RCSLastPunch;
    std::chrono::milliseconds LastAimTime;

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

            ImGui::Separator();

            if (ImGui::CollapsingHeader("Recoil Control", nullptr)) {
                ImGui::Checkbox("Recoil Control", &RecoilEnabled);
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                    ImGui::SetTooltip("Reduce the intensity of weapon's recoil.");
                ImGui::SliderFloat("Pitch", &PitchPower, 1, 10, "%.1f");
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                    ImGui::SetTooltip("Pitch Power");
                ImGui::SliderFloat("Yaw", &YawPower, 1, 10, "%.1f");
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                    ImGui::SetTooltip("Yaw Power");
            }

            ImGui::Separator();

            ImGui::Checkbox("Predict Movement", &PredictMovement);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Predict target's movement");
            ImGui::SameLine();
            ImGui::Checkbox("Predict Bullet Drop", &PredictBulletDrop);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Predict weapon's bullet drop");

            ImGui::Separator();

            ImGui::SliderFloat("Speed", &Speed, 1, 100, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Speed of the Aim-Assist\nHigher = Faster");

            ImGui::Separator();

            ImGui::SliderFloat("Smooth", &Smooth, 0, 0.99, "%.3f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Smoothness for the Aim-Assist\nHigher = Smoother");

            ImGui::Separator();

            ImGui::SliderFloat("FOV", &FOV, 1, 90, "%.0f");
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                ImGui::SetTooltip("Field of View");
            ImGui::SliderFloat("Zoom Scale", &ZoomScale, 0, 5, "%.1f");

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
            Config::Aimbot::PredictMovement = PredictMovement;
            Config::Aimbot::PredictBulletDrop = PredictBulletDrop;
            Config::Aimbot::Speed = Speed;
            Config::Aimbot::Smooth = Smooth;
            Config::Aimbot::FOV = FOV;
            Config::Aimbot::ZoomScale = ZoomScale;
            Config::Aimbot::MinDistance = MinDistance;
            Config::Aimbot::HipfireDistance = HipfireDistance;
            Config::Aimbot::ZoomDistance = ZoomDistance;
            Config::Aimbot::RecoilControl = RecoilEnabled;
            Config::Aimbot::PitchPower = PitchPower;
            Config::Aimbot::YawPower = YawPower;
            return true;
        } catch (...) {
            return false;
        }
    }

    void Update() {
        if (!AimbotEnabled) { ReleaseTarget(); return; }

        if (Myself->IsZooming)
            FinalDistance = ZoomDistance;
        else FinalDistance = HipfireDistance;

        if (!Myself->IsCombatReady()) { CurrentTarget = nullptr; return; }
        if (!X11Display->KeyDown(XK_Shift_L) && !Myself->IsInAttack) { ReleaseTarget(); return; }
        if (Myself->IsHoldingGrenade) { ReleaseTarget(); return; }

        Player* Target = CurrentTarget;
        if (!IsValidTarget(Target)) {
            if (TargetSelected)
                return;

            Target = FindBestTarget();
            if (!IsValidTarget(Target)) {
                ReleaseTarget();
                return;
            }
            
            CurrentTarget = Target;
            CurrentTarget->IsLockedOn = true;
            TargetSelected = true;
        } 
        
        if (TargetSelected && CurrentTarget) {
            std::chrono::milliseconds Now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
            if (Now >= LastAimTime + std::chrono::milliseconds(10)) {
                StartAiming();
                LastAimTime = Now + std::chrono::milliseconds((int)Utils::RandomRange(1, 10));
            }
            return;
        }
    }

    void StartAiming() {
        // Get Target Angle
        QAngle DesiredAngles = QAngle(0, 0);
        if (!GetAngle(CurrentTarget, DesiredAngles))
            return;

        // Recoil Control
        RecoilControl(DesiredAngles);

        // Smoothing
        SmoothAngle(DesiredAngles);

        // Aim angles
        Vector2D aimbotDelta = Vector2D(CalculatePitchIncrement(DesiredAngles), CalculateYawIncrement(DesiredAngles)).Multiply(Speed);
        int totalYawIncrementInt = RoundHalfEven(AL1AF0(aimbotDelta.x));
        int totalPitchIncrementInt = RoundHalfEven(AL1AF0(aimbotDelta.y * -1));

        // Move Mouse
        if (totalPitchIncrementInt == 0 && totalYawIncrementInt == 0) return;
        X11Display->MoveMouse(totalYawIncrementInt, totalPitchIncrementInt);
    }

    void SmoothAngle(QAngle& Angle) {
        QAngle ViewAngles = QAngle(Myself->ViewAngles.x, Myself->ViewAngles.y).NormalizeAngles();
        QAngle Delta = Angle - ViewAngles;
        Delta.NormalizeAngles();

        float SmoothValue = powf(this->Smooth, 0.4f);
        SmoothValue = std::min(0.99f, SmoothValue);

        QAngle ToChange = QAngle();
	    ToChange = Delta - Delta * Smooth;

        Angle = ViewAngles + ToChange;
    }

    bool GetAngle(Player* Target, QAngle& Angle) {
        const QAngle CurrentAngle = QAngle(Myself->ViewAngles.x, Myself->ViewAngles.y).NormalizeAngles();
        if (!CurrentAngle.isValid())
            return false;

        if (!GetAngleToTarget(Target, Angle))
            return false;

        return true;
    }

    bool GetAngleToTarget(Player* Target, QAngle& Angle) {
        Vector3D TargetPosition = Target->GetBonePosition(static_cast<HitboxType>(GetBestBone(Target)));
        Vector3D TargetVelocity = Target->AbsoluteVelocity;
        Vector3D CameraPosition = Myself->CameraPosition;
        QAngle CurrentAngle = QAngle(Myself->ViewAngles.x, Myself->ViewAngles.y).NormalizeAngles();
        
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

    double CalculateDistanceFromCrosshair(Vector3D TargetPosition) {
        Vector3D CameraPosition = Myself->CameraPosition;
        QAngle CurrentAngle = QAngle(Myself->ViewAngles.x, Myself->ViewAngles.y).NormalizeAngles();

        if (CameraPosition.Distance(TargetPosition) <= 0.0001f)
            return -1;

        QAngle TargetAngle = Resolver::CalculateAngle(CameraPosition, TargetPosition);
        if (!TargetAngle.isValid())
            return -1;
        
        return CurrentAngle.distanceTo(TargetAngle);
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

    float GetFOVScale() {
        if (Myself->IsValid()) {
            if (Myself->IsZooming) {
                if (Myself->TargetZoomFOV > 1.0 && Myself->TargetZoomFOV < 90.0) {
                    return tanf(DEG2RAD(Myself->TargetZoomFOV) * (0.64285714285));
                }
            }
        }
        return 1.0;
    }

    void RecoilControl(QAngle& Angle) {
        QAngle CurrentPunch = QAngle(Myself->PunchAngles.x, Myself->PunchAngles.y).NormalizeAngles();
        QAngle NewPunch = { CurrentPunch.x - RCSLastPunch.x, CurrentPunch.y - RCSLastPunch.y };

		Angle.x -= NewPunch.x * YawPower;
		Angle.y -= NewPunch.y * PitchPower;

        RCSLastPunch = CurrentPunch;
    }

    int GetBestBone(Player* Target) {
        float NearestDistance = 999;
        int NearestBone = 2;
        for (int i = 0; i < 6; i++) {
            HitboxType Bone = static_cast<HitboxType>(i);
            double DistanceFromCrosshair = CalculateDistanceFromCrosshair(Target->GetBonePosition(Bone));
            if (DistanceFromCrosshair < NearestDistance) {
                NearestBone = i;
                NearestDistance = DistanceFromCrosshair;
            }
        }
        return NearestBone;
    }

    Player* FindBestTarget() {
        Player* NearestTarget = nullptr;
        float BestDistance = 9999;
        float BestFOV = std::min(FOV, FOV * (GetFOVScale() * ZoomScale));
        Vector3D CameraPosition = Myself->CameraPosition;
        for (int i = 0; i < Players->size(); i++) {
            Player* p = Players->at(i);
            if (!IsValidTarget(p)) continue;
            if (p->DistanceToLocalPlayer < Conversion::ToGameUnits(ZoomDistance)) {
                HitboxType BestBone = static_cast<HitboxType>(GetBestBone(p));
                Vector3D TargetPosition = p->GetBonePosition(BestBone);

                float Distance = CameraPosition.Distance(TargetPosition);
                float FOV = CalculateDistanceFromCrosshair(TargetPosition);

                if (FOV > BestFOV) continue;
                if (Distance > BestDistance) continue;

                NearestTarget = p;
                BestDistance = Distance;
            }
        }
        return NearestTarget;
    }
};