#pragma once
#include <string>
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "UnityEngine/Sprite.hpp"

namespace CustomCharacteristics {

    GlobalNamespace::BeatmapCharacteristicSO* RegisterCustomCharacteristic(UnityEngine::Sprite* icon, std::string characteristicName, std::string hintText, std::string serializedName, std::string compoundIdPartName, bool requires360Movement = false, bool containsRotationEvents = false, int sortingOrder = 99);
    
    GlobalNamespace::BeatmapCharacteristicSO* FindByName(std::string characteristicName);

    void InstallHooks();

}