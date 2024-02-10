#pragma once
#include "Settings.h"

class Utility {
public:
    RE::BGSKeyword* keywordWarhammer;
    RE::BGSKeyword* keywordClothing;
    RE::BGSKeyword* keywordJewelry;
    RE::BGSKeyword* keywordMagicDisallow;

    RE::TESFaction* factionFollower1;
    RE::TESFaction* factionFollower2;

    RE::BGSLocationRefType* locationBoss;
    RE::BGSLocationRefType* locationBossContainer;

    std::vector<RE::BSFixedString> menuList;

    uintptr_t PlayerSingletonAddress;
    uintptr_t UISingletonAddress;
    uintptr_t MenuControlsSingletonAddress;

    static Utility* GetSingleton() {
        static Utility playerStatus;
        return &playerStatus; 
    }

    static RE::PlayerCharacter* GetPlayer() {
        REL::Relocation<RE::NiPointer<RE::PlayerCharacter>*> singleton{Utility::GetSingleton()->PlayerSingletonAddress};
        return singleton->get();
    }

    static RE::UI* GetUI() {
        REL::Relocation<RE::NiPointer<RE::UI>*> singleton{Utility::GetSingleton()->UISingletonAddress};
        return singleton->get();
    }

    static RE::MenuControls* GetMenuControls() {
        REL::Relocation<RE::NiPointer<RE::MenuControls>*> singleton{
            Utility::GetSingleton()->MenuControlsSingletonAddress};
        return singleton->get();
    }

    static void ShowNotification(std::string msg, bool messageBox = false, const char* a_soundToPlay = 0) {
        if (messageBox) {
            RE::DebugMessageBox(msg.c_str());
        } else {
            RE::DebugNotification(msg.c_str(), a_soundToPlay);
        }
    }

    static bool IsSystemMenu(RE::BSFixedString menuName) {
        auto menu = Utility::GetSingleton()->menuList;
        return (std::find(menu.begin(), menu.end(), menuName) != menu.end());
    }

    static bool IsSystemMenuOpen(RE::BSFixedString menuName = "") {
        auto ui = Utility::GetSingleton()->GetUI();
        auto menu = Utility::GetSingleton()->menuList;

        for (auto & element : menu) {
            if (menuName.c_str() != "" && std::strcmp(element.c_str(), menuName.c_str()))
                continue;

            if (ui->IsMenuOpen(element))
                return true;
        }

        return false;
    }

    // Player checks
    static bool IsPlayerInDialogue() {
        return Utility::GetSingleton()->GetUI()->IsMenuOpen(RE::DialogueMenu::MENU_NAME);
    }

    static bool PlayerNotInMenu() {
        auto ui = Utility::GetSingleton()->GetUI();

        if (ui && !ui->GameIsPaused() && !ui->IsApplicationMenuOpen() && !ui->IsItemMenuOpen() && !ui->IsMenuOpen(RE::InterfaceStrings::GetSingleton()->dialogueMenu))
            return true;
        else
            return false;
    }
    
    static void ForEachReferenceInRange(
        RE::TESObjectREFR* origin, float radius,
        std::function<RE::BSContainer::ForEachResult(RE::TESObjectREFR& ref)> callback) {
        if (origin && radius > 0.0f) {
            const auto originPos = origin->GetPosition();
            auto* tesSingleton = RE::TES::GetSingleton();
            auto* interiorCell = tesSingleton->interiorCell;
            if (interiorCell) {
                interiorCell->ForEachReferenceInRange(originPos, radius,
                                                      [&](RE::TESObjectREFR& a_ref) { return callback(a_ref); });
            } else {
                if (const auto gridLength = tesSingleton->gridCells ? tesSingleton->gridCells->length : 0;
                    gridLength > 0) {
                    const float yPlus = originPos.y + radius;
                    const float yMinus = originPos.y - radius;
                    const float xPlus = originPos.x + radius;
                    const float xMinus = originPos.x - radius;

                    std::uint32_t x = 0;
                    do {
                        std::uint32_t y = 0;
                        do {
                            if (const auto cell = tesSingleton->gridCells->GetCell(x, y); cell && cell->IsAttached()) {
                                if (const auto cellCoords = cell->GetCoordinates(); cellCoords) {
                                    const RE::NiPoint2 worldPos{cellCoords->worldX, cellCoords->worldY};
                                    if (worldPos.x < xPlus && (worldPos.x + 4096.0f) > xMinus && worldPos.y < yPlus &&
                                        (worldPos.y + 4096.0f) > yMinus) {
                                        cell->ForEachReferenceInRange(originPos, radius, [&](RE::TESObjectREFR& a_ref) {
                                            return callback(a_ref);
                                        });
                                    }
                                }
                            }
                            ++y;
                        } while (y < gridLength);
                        ++x;
                    } while (x < gridLength);
                }
            }
        } else {
            RE::TES::GetSingleton()->ForEachReference([&](RE::TESObjectREFR& a_ref) { return callback(a_ref); });
        }
    }
};
