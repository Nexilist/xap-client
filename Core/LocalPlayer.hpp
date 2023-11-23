#pragma once
#include "Offsets.hpp"
#include "../Utils/Memory.hpp"
#include "../Math/Vector2D.hpp"
#include "../Math/Vector3D.hpp"

struct LocalPlayer {
    long BasePointer;

    bool IsDead;
    bool IsInAttack;
    bool IsKnocked;
    bool IsZooming;

    int Team;
    Vector3D LocalOrigin;
    Vector3D CameraPosition;

    Vector2D ViewAngles;
    Vector2D PunchAngles;

    int WeaponIndex;
    float WeaponProjectileSpeed;
    float WeaponProjectileScale;
    bool IsHoldingGrenade;

    float ZoomFOV;
    float TargetZoomFOV;

    float ViewYaw;

    void ResetPointer() {
        BasePointer = 0;
    }

    void Read() {
        BasePointer = Memory::Read<long>(OFF_REGION + OFF_LOCAL_PLAYER);
        if (BasePointer == 0) return;

        IsDead = Memory::Read<short>(BasePointer + OFF_LIFE_STATE) > 0;
        IsKnocked = Memory::Read<short>(BasePointer + OFF_BLEEDOUT_STATE) > 0;
        IsZooming = Memory::Read<short>(BasePointer + OFF_ZOOMING) > 0;
        IsInAttack = Memory::Read<short>(OFF_REGION + OFF_INATTACK) > 0;

        Team = Memory::Read<int>(BasePointer + OFF_TEAM_NUMBER);
        LocalOrigin = Memory::Read<Vector3D>(BasePointer + OFF_LOCAL_ORIGIN);
        CameraPosition = Memory::Read<Vector3D>(BasePointer + OFF_CAMERAORIGIN);
        ViewAngles = Memory::Read<Vector2D>(BasePointer + OFF_VIEW_ANGLES);
        PunchAngles = Memory::Read<Vector2D>(BasePointer + OFF_PUNCH_ANGLES);

        ViewYaw = Memory::Read<float>(BasePointer + OFF_YAW);

        if (!IsDead && !IsKnocked) {
            long WeaponHandle = Memory::Read<long>(BasePointer + OFF_WEAPON_HANDLE);
            long WeaponHandleMasked = WeaponHandle & 0xffff;
            long WeaponEntity = Memory::Read<long>(OFF_REGION + OFF_ENTITY_LIST + (WeaponHandleMasked << 5));

            int OffHandWeaponID = Memory::Read<int>(BasePointer + OFF_OFFHAND_WEAPON);
            IsHoldingGrenade = OffHandWeaponID == -251 ? true : false;
            
            ZoomFOV = Memory::Read<float>(WeaponEntity + OFF_CURRENTZOOMFOV);
            TargetZoomFOV = Memory::Read<float>(WeaponEntity + OFF_TARGETZOOMFOV);
            
            WeaponIndex = Memory::Read<int>(WeaponEntity + OFF_WEAPON_INDEX);
            WeaponProjectileSpeed = Memory::Read<float>(WeaponEntity + OFF_PROJECTILESPEED);
            WeaponProjectileScale = Memory::Read<float>(WeaponEntity + OFF_PROJECTILESCALE);
        }
    }

    bool IsValid() {
        return BasePointer != 0;
    }

    bool IsCombatReady() {
        if (BasePointer == 0) return false;
        if (IsDead) return false;
        if (IsKnocked) return false;
        return true;
    }
};