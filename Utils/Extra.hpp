#pragma once
#include "../Core/Offsets.hpp"
#include "Memory.hpp"


void SkinChange()
    {
        if(!m_level->playable) return;
        if(m_localPlayer->dead) return;
        long wephandle = Memory::Read<long>(m_localPlayer->base + OFF_WEAPON_HANDLE);
        wephandle &= 0xffff;
        long wep_entity = m_localPlayer->weaponEntity;
        float curTime = Memory::Read<float>(m_localPlayer->base + OFFSET_TIME_BASE);
        float endTime = curTime +5.5;
        std::map<int, std::vector<int>> weaponSkinMap;
        //Light ammo weapons
        weaponSkinMap[104] = { 6 };   //WEAPON_P2020 
        weaponSkinMap[80] = { 6 };   //WEAPON_RE45 
        weaponSkinMap[79] = { 11 };   //WEAPON_ALTERNATOR 
        weaponSkinMap[103] = { 2 };   //WEAPON_R99  
        weaponSkinMap[0] = { 10 };     //WEAPON_R301   
        weaponSkinMap[105] = { 2 };    //WEAPON_SPITFIRE 
        weaponSkinMap[88] = { 5 };    //WEAPON_G7 
        //Heavy ammo weapons
        weaponSkinMap[111] = { 10};   // Car-SMG 
        weaponSkinMap[20] = { 6 };    // Rampage 
        weaponSkinMap[110] = { 9 };      //3030 
        weaponSkinMap[89] = {10 };   //WEAPON_HEMLOCK  
        weaponSkinMap[87] = { 8 };    //FlatLine  
        //Energy ammo weapons
        weaponSkinMap[112] = { 8 };    //WEAPON_NEMESIS  
        weaponSkinMap[109] = { 9 };    //WEAPON_VOLT 
        weaponSkinMap[106] = { 7 };    //WEAPON_TRIPLE_TAKE 
        weaponSkinMap[92] = { 3 };    //WEAPON_LSTAR 
        weaponSkinMap[83] = { 5 };    //WEAPON_DEVOTION 
        weaponSkinMap[85] = { 8 };    //WEAPON_HAVOC 
        //Sniper ammo weapons
        weaponSkinMap[1] = { 5 };    //WEAPON_SENTINEL 
        weaponSkinMap[82] = { 8 };    //WEAPON_CHARGE_RIFLE 
        weaponSkinMap[84] = { 7 };    //WEAPON_LONGBOW 
        //Shotgun ammo weapons
        weaponSkinMap[95] = { 5 };    //WEAPON_MOZAMBIQUE 
        weaponSkinMap[86] = { 8 };    //WEAPON_EVA8 
        weaponSkinMap[102] = { 7 };    //WEAPON_PEACEKEEPER 
        weaponSkinMap[94] = { 5 };    //WEAPON_MASTIFF 
        //Legendary ammo weapons
        weaponSkinMap[108] = { 5 };    //WEAPON_WINGMAN 
        weaponSkinMap[101] = { 7 };    //WEAPON_PROWLER
        weaponSkinMap[2] = { 3 };    //WEAPON_BOCEK
        weaponSkinMap[91] = { 6 };    //WEAPON_KRABER
        weaponSkinMap[158] = { 3 };    //WEAPON_THROWING_KNIFE
        weaponSkinMap[212] = { 2 };    //WEAPON_THERMITE_GRENADE 

        
            int waponIndex = Memory::Read<int>(wep_entity + OFF_WEAPON_INDEX);
            if (weaponSkinMap.count(waponIndex) == 0) return;
            int skinID = weaponSkinMap[waponIndex][0];
            //printf("Weapon: %s Activated Skin ID: %d \n", WeaponName(waponIndex).c_str(), skinID);  
            Memory::Write<int>(m_localPlayer->base + OFF_SKIN, skinID+1);
            Memory::Write<int>(wep_entity + OFF_SKIN, skinID);
            curTime = Memory::Read<float>(m_localPlayer->base + OFFSET_TIME_BASE);                 
    };