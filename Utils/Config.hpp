#pragma once
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
#include "IniReader.h"
#include <fstream>
#include <iostream>
#include <string>

constexpr char ConfigFile[] = "config.ini";

#define WriteSection(key)                                                      \
  conf << "[" #key "]"                                                         \
       << "\n";
#define WritePair(section, key)                                                \
  conf << #key " = " << Config::section::key << "\n";
#define WriteSectionEnd() conf << "\n";
#define WriteComment(msg) conf << "; " << msg << '\n';

#define ReadBool(section, key)                                                 \
  Config::section::key =                                                       \
      reader.GetBoolean(#section, #key, Config::section::key);
#define ReadFloat(section, key)                                                \
  Config::section::key = reader.GetFloat(#section, #key, Config::section::key);
#define ReadInt(section, key)                                                  \
  Config::section::key =                                                       \
      reader.GetInteger(#section, #key, Config::section::key);

namespace Config {
namespace Aimbot {
bool Enabled = true;
bool PredictMovement = true;
bool PredictBulletDrop = true;
bool RecoilControl = true;
float Speed = 15;
float Smooth = 5;
float FOV = 10;
float ZoomScale = 3.0;
float MinDistance = 1;
float HipfireDistance = 200;
float ZoomDistance = 200;
float PitchPower = 1;
float YawPower = 1;
}; // namespace Aimbot

namespace Sense {
bool Enabled = true;
bool ItemGlow = true;
bool DrawSeer = true;
bool AimedAtOnly = false;
float MaxDistance = 200;
bool ShowSpectators = true;
bool DrawFOVCircle = true;
float GameFOV = 120;
bool DrawBox = true;
bool Skeleton = true;
float ESPMaxDistance = 200;
bool ShowNear = true;
bool DrawNames = true;
bool Team = true;
}; // namespace Sense

namespace Triggerbot {
bool Enabled = true;
float Range = 200;
}; // namespace Triggerbot
}; // namespace Config

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
    WritePair(Sense, Team);
    WritePair(Sense, ItemGlow);
    WritePair(Sense, DrawSeer);
    WritePair(Sense, AimedAtOnly);
    WritePair(Sense, MaxDistance);
    WritePair(Sense, ShowSpectators);
    WritePair(Sense, DrawFOVCircle);
    WritePair(Sense, GameFOV);
    WritePair(Sense, DrawBox);
    WritePair(Sense, Skeleton);
    WritePair(Sense, ESPMaxDistance);
    WritePair(Sense, ShowNear);
    WritePair(Sense, DrawNames);
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
  ReadBool(Sense, Team);
  ReadBool(Sense, ItemGlow);
  ReadBool(Sense, DrawSeer);
  ReadBool(Sense, AimedAtOnly);
  ReadFloat(Sense, MaxDistance);
  ReadBool(Sense, ShowSpectators);
  ReadBool(Sense, DrawFOVCircle);
  ReadFloat(Sense, GameFOV);
  ReadBool(Sense, DrawBox);
  ReadBool(Sense, Skeleton);
  ReadFloat(Sense, ESPMaxDistance);
  ReadBool(Sense, ShowNear);
  ReadBool(Sense, DrawNames);

  ReadBool(Triggerbot, Enabled);
  ReadFloat(Triggerbot, Range);

  ReadBool(Aimbot, RecoilControl);
  ReadFloat(Aimbot, PitchPower);
  ReadFloat(Aimbot, YawPower);

  UpdateConfig();
  return true;
}