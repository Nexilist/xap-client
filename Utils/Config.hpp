#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include "IniReader.h"

constexpr char ConfigFile[] = "config.ini";

#define WriteSection(key) \
    conf << "[" #key "]" << "\n";
#define WritePair(section, key) \
    conf << #key " = " << Config::section::key << "\n";
#define WriteSectionEnd() conf << "\n";
#define WriteComment(msg) conf << "; " << msg << '\n';

#define ReadBool(section, key) \
    Config::section::key = reader.GetBoolean(#section, #key, Config::section::key);
#define ReadFloat(section, key) \
    Config::section::key = reader.GetFloat(#section, #key, Config::section::key);
#define ReadInt(section, key) \
    Config::section::key = reader.GetInteger(#section, #key, Config::section::key);

namespace Config {
    namespace Aimbot {
        bool Enabled = true;
        bool WInScope = true;
        bool PredictMovement = true;
        bool PredictBulletDrop = true;
        bool RecoilControl = true;
        float Speed = 40;
        float Smooth = 10;
        float ExtraSmooth = 250;
        float FOV = 10;
        float ZoomScale = 1.2;
        float MinDistance = 1;
        float HipfireDistance = 200;
        float ZoomDistance = 200;
        float PitchPower = 1;
        float YawPower = 1;
    };

    namespace Glow {
        bool Enabled = true;
        bool ItemGlow = true;
        bool DrawSeer = true;
        bool VisibleOnly = true;
        float MaxDistance = 200;
        float SeerMaxDistance = 200;
        bool DrawFOVCircle = true;
        float GameFOV = 120;
    };

    namespace Triggerbot {
        bool Enabled = true;
        float Range = 200;
    };
};

void UpdateConfig() {
    std::ofstream conf(ConfigFile);
    if (conf.is_open()) {
        WriteSection(Aimbot);
        WritePair(Aimbot, Enabled);
        WritePair(Aimbot, WInScope);
        WritePair(Aimbot, PredictMovement);
        WritePair(Aimbot, PredictBulletDrop);
        WritePair(Aimbot, Speed);
        WritePair(Aimbot, Smooth);
        WritePair(Aimbot, ExtraSmooth);
        WritePair(Aimbot, FOV);
        WritePair(Aimbot, ZoomScale);
        WritePair(Aimbot, MinDistance);
        WritePair(Aimbot, HipfireDistance);
        WritePair(Aimbot, ZoomDistance);
        WritePair(Aimbot, RecoilControl);
        WritePair(Aimbot, PitchPower);
        WritePair(Aimbot, YawPower);
        WriteSectionEnd();

        WriteSection(Glow);
        WritePair(Glow, Enabled);
        WritePair(Glow, ItemGlow);
        WritePair(Glow, DrawSeer);
        WritePair(Glow, VisibleOnly);
        WritePair(Glow, MaxDistance);
        WritePair(Glow, SeerMaxDistance);
        WritePair(Glow, DrawFOVCircle);
        WritePair(Glow, GameFOV);
        WriteSectionEnd();

        WriteSection(Triggerbot);
        WritePair(Triggerbot, Enabled);
        WritePair(Triggerbot, Range);
        WriteSectionEnd();
        conf.close();
    }
}

bool ReadConfig(const std::string &configFile) {
    INIReader reader(configFile);
    if (reader.ParseError() < 0) {
        UpdateConfig();
        return false;
    }
    
    ReadBool(Aimbot, Enabled);
    ReadBool(Aimbot, WInScope);
    ReadBool(Aimbot, PredictMovement);
    ReadBool(Aimbot, PredictBulletDrop);
    ReadFloat(Aimbot, Speed);
    ReadFloat(Aimbot, Smooth);
    ReadFloat(Aimbot, ExtraSmooth);
    ReadFloat(Aimbot, FOV);
    ReadFloat(Aimbot, ZoomScale);
    ReadFloat(Aimbot, MinDistance);
    ReadFloat(Aimbot, HipfireDistance);
    ReadFloat(Aimbot, ZoomDistance);

    ReadBool(Glow, Enabled);
    ReadBool(Glow, ItemGlow);
    ReadBool(Glow, DrawSeer);
    ReadBool(Glow, VisibleOnly);
    ReadFloat(Glow, MaxDistance);
    ReadFloat(Glow, SeerMaxDistance);
    ReadBool(Glow, DrawFOVCircle);
    ReadFloat(Glow, GameFOV);

    ReadBool(Triggerbot, Enabled);
    ReadFloat(Triggerbot, Range);

    ReadBool(Aimbot, RecoilControl);
    ReadFloat(Aimbot, PitchPower);
    ReadFloat(Aimbot, YawPower);

    UpdateConfig();
    return true;
}
