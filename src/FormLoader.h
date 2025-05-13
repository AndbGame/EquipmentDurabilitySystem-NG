#pragma once

#include "Utility.h"
#undef AddForm

class FormLoader {
public:
    const std::string_view pluginSkyrim = "Skyrim.esm";
    const std::string_view pluginUpdate = "Update.esm";
    const std::string_view pluginDawnguard = "Dawnguard.esm";
    const std::string_view pluginHeathfire = "HearthFires.esm";

    static FormLoader* GetSingleton() {
        static FormLoader formLoader;
        return &formLoader;
    }

    void LoadAllForms();

    void loadMenus();

    void LoadMiscForms([[maybe_unused]] RE::TESDataHandler* dataHandler);

    // Cache commonly called addresses to avoid address lib overhead
    void CacheGameAddresses();
};