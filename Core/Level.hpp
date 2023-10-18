#pragma once
#include "Offsets.hpp"
#include "../Utils/Memory.hpp"

struct Level {
    std::string Name;
    bool IsPlayable;
    bool IsFiringRange;

    void Read() {
        Name = Memory::ReadString(OFF_REGION + OFF_LEVEL);
        IsPlayable = !Name.empty() && Name != "mp_lobby";
        IsFiringRange = Name == "mp_rr_canyonlands_staging_mu1";
    }
};