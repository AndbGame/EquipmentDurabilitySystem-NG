#include "FormLoader.h"

void FormLoader::LoadAllForms() {
    logger::info("Loading all forms.");

    const auto dataHandler = RE::TESDataHandler::GetSingleton();

    LoadMiscForms(dataHandler);
    logger::info("All forms are loaded.");

    loadMenus();
    logger::info("All menu names are loaded.");
}

void FormLoader::loadMenus() {
    auto utility = Utility::GetSingleton();
    utility->menuList.push_back(RE::MainMenu::MENU_NAME);
    utility->menuList.push_back(RE::DialogueMenu::MENU_NAME);
    utility->menuList.push_back(RE::CraftingMenu::MENU_NAME);
    utility->menuList.push_back(RE::MagicMenu::MENU_NAME);
    utility->menuList.push_back(RE::LockpickingMenu::MENU_NAME);
    utility->menuList.push_back(RE::InventoryMenu::MENU_NAME);
    utility->menuList.push_back(RE::LoadingMenu::MENU_NAME);
    utility->menuList.push_back(RE::JournalMenu::MENU_NAME);
    utility->menuList.push_back(RE::BarterMenu::MENU_NAME);
    utility->menuList.push_back(RE::ContainerMenu::MENU_NAME);
    utility->menuList.push_back(RE::BookMenu::MENU_NAME);
    utility->menuList.push_back(RE::MapMenu::MENU_NAME);
    utility->menuList.push_back(RE::GiftMenu::MENU_NAME);
    utility->menuList.push_back(RE::FavoritesMenu::MENU_NAME);
    utility->menuList.push_back(RE::CreationClubMenu::MENU_NAME);
    utility->menuList.push_back(RE::ModManagerMenu::MENU_NAME);
    utility->menuList.push_back(RE::RaceSexMenu::MENU_NAME);
    utility->menuList.push_back(RE::StatsMenu::MENU_NAME);
    utility->menuList.push_back(RE::TrainingMenu::MENU_NAME);
    utility->menuList.push_back(RE::TutorialMenu::MENU_NAME);
    utility->menuList.push_back(RE::TweenMenu::MENU_NAME);
    utility->menuList.push_back(RE::CursorMenu::MENU_NAME);
}

void FormLoader::LoadMiscForms([[maybe_unused]] RE::TESDataHandler* dataHandler) {
    auto utility = Utility::GetSingleton();

    // Keywords
    utility->keywordWarhammer = dataHandler->LookupForm(RE::FormID(0x06D930), pluginSkyrim)->As<RE::BGSKeyword>();
    utility->keywordClothing = dataHandler->LookupForm(RE::FormID(0x08F95B), pluginSkyrim)->As<RE::BGSKeyword>();
    utility->keywordJewelry = dataHandler->LookupForm(RE::FormID(0x08F95A), pluginSkyrim)->As<RE::BGSKeyword>();
    utility->keywordMagicDisallow = dataHandler->LookupForm(RE::FormID(0x0C27BD), pluginSkyrim)->As<RE::BGSKeyword>();

    // Follower Factions
    utility->factionFollower1 = dataHandler->LookupForm(RE::FormID(0x05C84D), pluginSkyrim)->As<RE::TESFaction>();
    utility->factionFollower2 = dataHandler->LookupForm(RE::FormID(0x09A7B2), pluginSkyrim)->As<RE::TESFaction>();

    // Locations
    utility->locationBoss = dataHandler->LookupForm(RE::FormID(0x0130F7), pluginSkyrim)->As<RE::BGSLocationRefType>();
    utility->locationBossContainer = dataHandler->LookupForm(RE::FormID(0x0130F8), pluginSkyrim)->As<RE::BGSLocationRefType>();
}

// Cache commonly called addresses to avoid address lib overhead
void FormLoader::CacheGameAddresses() {
    auto utility = Utility::GetSingleton();

    // Cache player singleton address
    utility->PlayerSingletonAddress = RELOCATION_ID(517014, 403521).address();
    utility->UISingletonAddress = RELOCATION_ID(514178, 400327).address();
    utility->MenuControlsSingletonAddress = RELOCATION_ID(515124, 401263).address();
}