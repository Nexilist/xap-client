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
        bool AllowTargetSwitch = true;
        bool PredictMovement = true;
        bool PredictBulletDrop = true;
        int Hitbox = 1;
        float Speed = 40;
        float Smooth = 10;
        float ExtraSmooth = 250;
        float FOV = 10;
        float MinDistance = 2;
        float MaxDistance = 200;
    };

    namespace Glow {
        bool Enabled = true;
        bool ItemGlow = true;
        float MaxDistance = 200;
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
        WritePair(Aimbot, AllowTargetSwitch);
        WritePair(Aimbot, PredictMovement);
        WritePair(Aimbot, PredictBulletDrop);
        WritePair(Aimbot, Hitbox);
        WritePair(Aimbot, Speed);
        WritePair(Aimbot, Smooth);
        WritePair(Aimbot, ExtraSmooth);
        WritePair(Aimbot, FOV);
        WritePair(Aimbot, MinDistance);
        WritePair(Aimbot, MaxDistance);
        WriteSectionEnd();

        WriteSection(Glow);
        WritePair(Glow, Enabled);
        WritePair(Glow, ItemGlow);
        WritePair(Glow, MaxDistance);
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
    ReadBool(Aimbot, AllowTargetSwitch);
    ReadBool(Aimbot, PredictMovement);
    ReadBool(Aimbot, PredictBulletDrop);
    ReadInt(Aimbot, Hitbox);
    ReadFloat(Aimbot, Speed);
    ReadFloat(Aimbot, Smooth);
    ReadFloat(Aimbot, ExtraSmooth);
    ReadFloat(Aimbot, FOV);
    ReadFloat(Aimbot, MinDistance);
    ReadFloat(Aimbot, MaxDistance);

    ReadBool(Glow, Enabled);
    ReadBool(Glow, ItemGlow);
    ReadFloat(Glow, MaxDistance);

    ReadBool(Triggerbot, Enabled);
    ReadFloat(Triggerbot, Range);

    UpdateConfig();
    return true;
}