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
        bool PredictMovement = true;
        bool PredictBulletDrop = true;
        bool RecoilControl = true;
        float Speed = 6;
        float Smooth = 19;
        float ExtraSmooth = 1144;
        float FOV = 5;
        float ZoomScale = 1.4;

        float MinDistance = 1;
        float HipfireDistance = 27;
        float ZoomDistance = 200;
        float PitchPower = 2.32465123;
        float YawPower = 1.534342324;
    };

    namespace Sense {
        bool Enabled = true;
        bool ItemGlow = true;
        bool DrawSeer = true;
        bool VisibleOnly = false;
        float MaxDistance = 500;
        float SeerMaxDistance = 500;
        bool DrawFOVCircle = false;
        float GameFOV = 110;
        bool ShowSpectators = true;

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
        WritePair(Aimbot, PredictMovement);
        WritePair(Aimbot, PredictBulletDrop);
        WritePair(Aimbot, Speed);
        WritePair(Aimbot, Smooth);
        WritePair(Aimbot, FOV);
        WritePair(Aimbot, ZoomScale);
        WritePair(Aimbot, MinDistance);
        WritePair(Aimbot, HipfireDistance);
        WritePair(Aimbot, ZoomDistance);
        WritePair(Aimbot, RecoilControl);
        WritePair(Aimbot, PitchPower);
        WritePair(Aimbot, YawPower);
        WriteSectionEnd();

        WriteSection(Sense);
        WritePair(Sense, Enabled);
        WritePair(Sense, ItemGlow);
        WritePair(Sense, DrawSeer);
        WritePair(Sense, AimedAtOnly);
        WritePair(Sense, MaxDistance);
        WritePair(Sense, SeerMaxDistance);
        WritePair(Sense, ShowSpectators);
        WritePair(Sense, DrawFOVCircle);
        WritePair(Sense, GameFOV);
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
    ReadBool(Aimbot, PredictMovement);
    ReadBool(Aimbot, PredictBulletDrop);
    ReadFloat(Aimbot, Speed);
    ReadFloat(Aimbot, Smooth);
    ReadFloat(Aimbot, FOV);
    ReadFloat(Aimbot, ZoomScale);
    ReadFloat(Aimbot, MinDistance);
    ReadFloat(Aimbot, HipfireDistance);
    ReadFloat(Aimbot, ZoomDistance);

    ReadBool(Sense, Enabled);
    ReadBool(Sense, ItemGlow);
    ReadBool(Sense, DrawSeer);
    ReadBool(Sense, AimedAtOnly);
    ReadFloat(Sense, MaxDistance);
    ReadFloat(Sense, SeerMaxDistance);
    ReadBool(Sense, ShowSpectators);
    ReadBool(Sense, DrawFOVCircle);
    ReadFloat(Sense, GameFOV);

    ReadBool(Triggerbot, Enabled);
    ReadFloat(Triggerbot, Range);

    ReadBool(Aimbot, RecoilControl);
    ReadFloat(Aimbot, PitchPower);
    ReadFloat(Aimbot, YawPower);

    UpdateConfig();
    return true;
}