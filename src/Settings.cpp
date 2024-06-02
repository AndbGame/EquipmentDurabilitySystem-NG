#include "Settings.h"
#include "Utility.h"

INIFile ini;

constexpr auto settings_path = L"Data/SKSE/Plugins/EquipmentDurability/EquipmentDurability.ini";
constexpr auto break_path = L"Data/SKSE/Plugins/EquipmentDurability/NoBreakForms.ini";
constexpr auto enchant_path = L"Data/SKSE/Plugins/EquipmentDurability/";

INIFile::INIFile() : EquipmentHealthThreshold(1.0f) {
	degradationMap["disabledegradation"] = 0;
	degradationMap["onlyplayer"] = 1;
	degradationMap["nobreakmagicdisallowenchanting"] = 1;
	degradationMap["increasedurability"] = 1;
	degradationMap["equipmenthealththreshold"] = 0;
	degradationMap["materialmultiplier"] = 0;

	degradationRateMap["weaponsword"] = 3.0;
	degradationRateMap["weapondagger"] = 3.0;
	degradationRateMap["weaponwaraxe"] = 3.0;
	degradationRateMap["weaponmace"] = 3.0;
	degradationRateMap["weapongreatsword"] = 3.0;
	degradationRateMap["weaponhammer"] = 3.0;
	degradationRateMap["weaponbattleaxe"] = 3.0;
	degradationRateMap["weaponbow"] = 3.0;
	degradationRateMap["weaponcrossbow"] = 3.0;
	degradationRateMap["lightarmor"] = 3.0;
	degradationRateMap["heavyarmor"] = 3.0;
	degradationRateMap["clothing"] = 3.0;
	degradationRateMap["defaultarmor"] = 3.0;
	degradationRateMap["powerattackmultiplier"] = 2.0;
	degradationRateMap["followermultiplier"] = 1.0;
	degradationRateMap["npcmultiplier"] = 1.0;

	breakChanceMap["weaponsword"] = 0.0;
	breakChanceMap["weapondagger"] = 0.0;
	breakChanceMap["weaponwaraxe"] = 0.0;
	breakChanceMap["weaponmace"] = 0.0;
	breakChanceMap["weapongreatsword"] = 0.0;
	breakChanceMap["weaponhammer"] = 0.0;
	breakChanceMap["weaponbattleaxe"] = 0.0;
	breakChanceMap["weaponbow"] = 0.0;
	breakChanceMap["weaponcrossbow"] = 0.0;
	breakChanceMap["lightarmor"] = 0.0;
	breakChanceMap["heavyarmor"] = 0.0;
	breakChanceMap["clothing"] = 0.0;
	breakChanceMap["defaultarmor"] = 0.0;
	breakChanceMap["powerattackmultiplier"] = 2.0;
	breakChanceMap["followermultiplier"] = 1.0;
	breakChanceMap["npcmultiplier"] = 1.0;

	temperMap["disabledynamictemper"] = 0;
	temperMap["temperchance"] = 40;
	temperMap["vendortemperchance"] = 50;
	temperMap["bosstemperchance"] = 80;

	enchantMap["disabledynamicenchant"] = 0;
	enchantMap["enchantchance"] = 20;
	enchantMap["vendorenchantchance"] = 30;
	enchantMap["bossenchantchance"] = 60;

	widgetMap["disablewidget"] = 0;
	widgetMap["positionx"] = 3;
	widgetMap["positiony"] = 73;
	widgetMap["scale"] = 100;
	widgetMap["unbreakablecolor"] = 0xFFFFFF;
	widgetMap["breakablecolor"] = 0xFFD700;
	widgetMap["hidepoisonname"] = 0;
	widgetMap["hidewithweaponssheathed"] = 0;
	widgetMap["togglekeycode"] = 0;
	widgetMap["toggleduration"] = 0.0;
	widgetMap["flags"] = 8;
}

#pragma region "Public Functions"

void INIFile::Load() {
	SetSettings();
	ShowSettings();
	SetVendorList();

	if (degradationMap["equipmenthealththreshold"] == 0)
		EquipmentHealthThreshold = 1.0f;
	else
		EquipmentHealthThreshold = degradationMap["equipmenthealththreshold"] * 0.001f + 1.0f;
}

bool INIFile::IsVendorContainer(RE::TESObjectREFR* form) {
	if (std::find(vendorContainers.begin(), vendorContainers.end(), form) != vendorContainers.end())
		return true;
	else
		return false;
}

int INIFile::GetDegradationSettings(std::string str) {
	std::string tmp = str;
	ToLower(tmp);
	return 	(degradationMap.count(tmp) >= 1) ? degradationMap.at(tmp) : 0;
}

double INIFile::GetDegradationRateSettings(std::string str) {
	std::string tmp = str;
	ToLower(tmp);
	return 	(degradationRateMap.count(tmp) >= 1) ? degradationRateMap.at(tmp) : 0.0;
}

double INIFile::GetDegradationRateSettings(RE::TESForm* form) {
	auto utility = Utility::GetSingleton();

	if (form->formType == RE::FormType::Weapon) {
		RE::TESObjectWEAP *weap = form->As<RE::TESObjectWEAP>();

		// Material Check
		double matMulti = MaterialRate(weap->GetKeywords());

		// Default Degredation Rate
		switch (weap->GetWeaponType()) {
			case RE::WEAPON_TYPE::kOneHandSword:
				return GetDegradationRateSettings("WeaponSword") * matMulti;
			case RE::WEAPON_TYPE::kOneHandDagger:
				return GetDegradationRateSettings("WeaponDagger") * matMulti;
			case RE::WEAPON_TYPE::kOneHandAxe:
				return GetDegradationRateSettings("WeaponWarAxe") * matMulti;
			case RE::WEAPON_TYPE::kOneHandMace:
				return GetDegradationRateSettings("WeaponMace") * matMulti;
			case RE::WEAPON_TYPE::kTwoHandSword:
				return GetDegradationRateSettings("WeaponGreatSword") * matMulti;
			case RE::WEAPON_TYPE::kTwoHandAxe:
				if (weap->HasKeyword(utility->keywordWarhammer))
					return GetDegradationRateSettings("WeaponHammer") * matMulti;
				else
					return GetDegradationRateSettings("WeaponBattleAxe") * matMulti;
			case RE::WEAPON_TYPE::kBow:
				return GetDegradationRateSettings("WeaponBow") * matMulti;
			case RE::WEAPON_TYPE::kCrossbow:
				return GetDegradationRateSettings("WeaponCrossbow") * matMulti;
			default:
				return GetDegradationRateSettings("WeaponSword") * matMulti;
		}
	} else if (form->formType == RE::FormType::Armor) {
		RE::TESObjectARMO *armor = form->As<RE::TESObjectARMO>();

		// Material Check
		double matMulti = MaterialRate(armor->GetKeywords());

		// Default Degredation Rate
		if (armor->IsLightArmor())
			return GetDegradationRateSettings("LightArmor") * matMulti;
		else if (armor->IsHeavyArmor())
			return GetDegradationRateSettings("HeavyArmor") * matMulti;
		else if (armor->HasKeyword(utility->keywordClothing))
			return GetDegradationRateSettings("Clothing") * matMulti;
	}

	return GetDegradationRateSettings("DefaultArmor");
}

double INIFile::GetBreakChanceSettings(std::string str) {
	std::string tmp = str;
	ToLower(tmp);
	return (breakChanceMap.count(tmp) >= 1) ? breakChanceMap.at(tmp) : 0.0;
}

double INIFile::GetBreakChanceSettings(RE::TESForm* form) {
	auto utility = Utility::GetSingleton();

	if (form->formType == RE::FormType::Weapon) {
		RE::TESObjectWEAP *weap = form->As<RE::TESObjectWEAP>();

		// Material Check
		double matMulti = MaterialRate(weap->GetKeywords());

		// Default Break Chance
		switch (weap->GetWeaponType()) {
			case RE::WEAPON_TYPE::kOneHandSword:
				return GetBreakChanceSettings("WeaponSword") * matMulti;
			case RE::WEAPON_TYPE::kOneHandDagger:
				return GetBreakChanceSettings("WeaponDagger") * matMulti;
			case RE::WEAPON_TYPE::kOneHandAxe:
				return GetBreakChanceSettings("WeaponWarAxe") * matMulti;
			case RE::WEAPON_TYPE::kOneHandMace:
				return GetBreakChanceSettings("WeaponMace") * matMulti;
			case RE::WEAPON_TYPE::kTwoHandSword:
				return GetBreakChanceSettings("WeaponGreatSword") * matMulti;
			case RE::WEAPON_TYPE::kTwoHandAxe:
				if (weap->HasKeyword(utility->keywordWarhammer))
					return GetBreakChanceSettings("WeaponHammer") * matMulti;
				else
					return GetBreakChanceSettings("WeaponBattleAxe") * matMulti;
			case RE::WEAPON_TYPE::kBow:
				return GetBreakChanceSettings("WeaponBow") * matMulti;
			case RE::WEAPON_TYPE::kCrossbow:
				return GetBreakChanceSettings("WeaponCrossbow") * matMulti;
			default:
				return GetBreakChanceSettings("WeaponSword") * matMulti;
		}
	} else if (form->formType == RE::FormType::Armor) {
		RE::TESObjectARMO *armor = form->As<RE::TESObjectARMO>();

		// Material Check
		double matMulti = MaterialRate(armor->GetKeywords());

		// Default Break Chance
		if (armor->IsLightArmor())
			return GetBreakChanceSettings("LightArmor") * matMulti;
		else if (armor->IsHeavyArmor())
			return GetBreakChanceSettings("HeavyArmor") * matMulti;
		else if (armor->HasKeyword(utility->keywordClothing))
			return GetBreakChanceSettings("Clothing") * matMulti;
	}

	return GetBreakChanceSettings("DefaultArmor");
}

double INIFile::MaterialRate(std::span<RE::BGSKeyword*> keywords) {
	int iKeywordCount = 0;
	double dRates = 0;

	// If Material Multiplier is on
	if (GetDegradationSettings("MaterialMultiplier") >= 1) {
		for (RE::BGSKeyword* keyword : keywords) {
			std::string tmp = keyword->GetFormEditorID();
			ToLower(tmp);

			if (materialMap.count(tmp) >= 1) {
				dRates += materialMap.at(tmp);
				iKeywordCount++;
			}
		}

		double finalRate = dRates / iKeywordCount;
		return (finalRate > 0) ? finalRate : 1.0;

	} else
		return 1.0;
}

int INIFile::GetTemperSettings(std::string str) {
	std::string tmp = str;
	ToLower(tmp);
	return 	(temperMap.count(tmp) >= 1) ? temperMap.at(tmp) : 0;
}

int INIFile::GetEnchantSettings(std::string str) {
	std::string tmp = str;
	ToLower(tmp);
	return 	(enchantMap.count(tmp) >= 1) ? enchantMap.at(tmp) : 0;
}

int INIFile::GetWidgetSettings(std::string str)
{
	std::string tmp = str;
	ToLower(tmp);
	return 	(widgetMap.count(tmp) >= 1) ? widgetMap.at(tmp) : 0;
}

int INIFile::GetEnchantmentSize(std::string part) {
	if (stricmp(part.c_str(),"weapon"))
		return enchantWeapon.size();
	else if (stricmp(part.c_str(),"head"))
		return enchantHead.size();
	else if (stricmp(part.c_str(),"body"))
		return enchantBody.size();
	else if (stricmp(part.c_str(),"hand"))
		return enchantHand.size();
	else if (stricmp(part.c_str(),"foot"))
		return enchantFoot.size();
	else if (stricmp(part.c_str(),"shield"))
		return enchantShield.size();

	return 0;
}

std::vector<Enchantments>* INIFile::GetEnchantmentList(std::string part) {
	// Select correct vector based on part
	if (stricmp(part.c_str(),"weapon"))
		return &enchantWeapon;
	else if (stricmp(part.c_str(),"head"))
		return &enchantHead;
	else if (stricmp(part.c_str(),"body"))
		return &enchantBody;
	else if (stricmp(part.c_str(),"hand"))
		return &enchantHand;
	else if (stricmp(part.c_str(),"foot"))
		return &enchantFoot;
	else if (stricmp(part.c_str(),"shield"))
		return &enchantShield;

	return nullptr;
}

RE::EnchantmentItem* INIFile::GetEnchantmentForm(std::string part, int index) {
	Enchantments foundEnchant;

	// Add the enchantment to the correct lists
	if (stricmp(part.c_str(),"weapon"))
		foundEnchant = enchantWeapon.at(index);
	else if (stricmp(part.c_str(),"head"))
		foundEnchant = enchantHead.at(index);
	else if (stricmp(part.c_str(),"body"))
		foundEnchant = enchantBody.at(index);
	else if (stricmp(part.c_str(),"hand"))
		foundEnchant = enchantHand.at(index);
	else if (stricmp(part.c_str(),"foot"))
		foundEnchant = enchantFoot.at(index);
	else if (stricmp(part.c_str(),"shield"))
		foundEnchant = enchantShield.at(index);

	return foundEnchant.enchantment;
}

std::string INIFile::GetEnchantmentName(std::string part, int index) {
	Enchantments foundEnchant;

	// Add the enchantment to the correct lists
	if (stricmp(part.c_str(),"weapon"))
		foundEnchant = enchantWeapon.at(index);
	else if (stricmp(part.c_str(),"head"))
		foundEnchant = enchantHead.at(index);
	else if (stricmp(part.c_str(),"body"))
		foundEnchant = enchantBody.at(index);
	else if (stricmp(part.c_str(),"hand"))
		foundEnchant = enchantHand.at(index);
	else if (stricmp(part.c_str(),"foot"))
		foundEnchant = enchantFoot.at(index);
	else if (stricmp(part.c_str(),"shield"))
		foundEnchant = enchantShield.at(index);

	return foundEnchant.name;
}

bool INIFile::HasNoBreakForms(int formid) {
	if (noBreakForms.empty())
		return false;

	auto it = noBreakForms.find(formid);
	if (it == noBreakForms.end())
		return false;

	return true;
}

#pragma endregion "Public Functions"

void INIFile::ToLower(std::string &str)
{
	for (auto &c : str)
		c = tolower(c);
}

bool INIFile::stricmp(const char* str_1, const char* str_2) {
	// Create locals and lower case them
	std::string a(str_1);
	ToLower(a);
	std::string b(str_2);
	ToLower(b);

	// Compare the values and return true/false
	if(a.compare(b) == 0)
		return true;
	else
		return false;
}

std::vector<std::string> INIFile::split(std::string str, char delim) {
	std::stringstream test(str);
	std::string segment;
	std::vector<std::string> seglist;

	while(std::getline(test, segment, delim)) {
	   seglist.push_back(segment);
	}

	return seglist;
}

void INIFile::SetSettings() {
	// System Settings
	CSimpleIniA iniSettings;
	iniSettings.SetUnicode();
	iniSettings.LoadFile(settings_path);

	std::list<CSimpleIniA::Entry> i_sections;
	iniSettings.GetAllSections(i_sections);

	for (CSimpleIniA::Entry i_section : i_sections)  {
		std::list<CSimpleIniA::Entry> entryList;
		iniSettings.GetAllKeys(i_section.pItem, entryList);

		if (stricmp(i_section.pItem, "DegradationRate"))
			SetINIData1(&entryList, i_section.pItem, &degradationRateMap, &iniSettings);
		else if (stricmp(i_section.pItem, "BreakChance"))
			SetINIData1(&entryList, i_section.pItem, &breakChanceMap, &iniSettings);
		else if (stricmp(i_section.pItem, "MaterialMultiplier"))
			SetINIData1(&entryList, i_section.pItem, &materialMap, &iniSettings);
		else
			SetINIData1(&entryList, i_section.pItem, &iniSettings);
	}

	// Break Forms
	CSimpleIniA iniBreak;
	iniBreak.SetUnicode();
	iniBreak.LoadFile(break_path);

	std::list<CSimpleIniA::Entry> sectionList;
	iniBreak.GetAllSections(sectionList);

	SetINIData2(&sectionList, &noBreakForms, &iniBreak);

	// Enchantment Forms
	std::vector<std::string> configs{};
	std::string sfilename = "_ENCH";

	for (const auto iterator = std::filesystem::directory_iterator(enchant_path); const auto& entry : iterator) {
		if (entry.exists()) {
			if (const auto& path = entry.path(); !path.empty() && path.extension() == ".ini") {
				if (const auto& fileName = entry.path().string(); sfilename.empty() || fileName.rfind(sfilename) != std::string::npos) {
					configs.push_back(fileName);
				}
			}
		}
	}
	std::ranges::sort(configs);

	if (configs.empty()) {
		logger::debug("No .ini files with _ENCH suffix were found within the Data folder, aborting...");
		return;
	}

	logger::info("{} matching files found", configs.size());
	for (auto& path : configs) {
		logger::info("Enchant File : {}", path);

		CSimpleIniA iniEnch;
		iniEnch.SetUnicode();
		iniEnch.SetMultiKey();
		iniEnch.SetAllowKeyOnly();

		SI_Error rc = iniEnch.LoadFile(path.c_str());
		if (rc < 0) {
			logger::error("\tcouldn't read INI");
			continue;
		}

		std::list<CSimpleIniA::Entry> sections;
		iniEnch.GetAllSections(sections);
		sections.sort(CSimpleIniA::Entry::LoadOrder());

		std::wstring path_w = std::wstring(path.begin(), path.end());
		SetINIData3(&sectionList, path_w.c_str(), &iniEnch);
	}

}

void INIFile::SetINIData1(std::list<CSimpleIniA::Entry> *list, const char* section, CSimpleIniA *iniSettings) {
	for (CSimpleIniA::Entry str : *list) {
		std::string key(str.pItem);
		ToLower(key);

		if (!key.empty()) {
			auto cValue = iniSettings->GetValue(section, str.pItem);
			std::string value = iniSettings->GetValue(section, str.pItem);
			ToLower(value);

			int iValue = 0;
			if (stricmp(cValue,"true"))
				iValue = 1;

			if (stricmp(str.pItem,"disabledegradation") || stricmp(str.pItem,"onlyplayer") || stricmp(str.pItem,"nobreakmagicdisallowenchanting") || stricmp(str.pItem,"increasedurability") || stricmp(str.pItem,"MaterialMultiplier"))
				degradationMap.at(key) = iValue;
			else if (stricmp(str.pItem,"disabledynamictemper"))
				temperMap.at(key) = iValue;
			else if (stricmp(str.pItem,"disabledynamicenchant"))
				enchantMap.at(key) = iValue;
			else if (stricmp(str.pItem,"disablewidget") || stricmp(str.pItem,"hidepoisonname") || stricmp(str.pItem,"hidewithweaponssheathed"))
				widgetMap.at(key) = iValue;
			else {
				iValue = 0;
				try {
					iValue = std::stoi(value, nullptr, 0);
				} catch (const std::invalid_argument& e) {
					logger::debug("{} : invalid argument", key);
					logger::debug("   {}", e.what());
				} catch (const std::out_of_range& e) {
					logger::debug("{} : out of range", key);
					logger::debug("   {}", e.what());
				}

				if (stricmp(str.pItem,"equipmenthealththreshold")) {
					degradationMap.at(key) = iValue;
				} else if (stricmp(str.pItem,"temperchance") || stricmp(str.pItem,"vendortemperchance") || stricmp(str.pItem,"bosstemperchance")) {
					temperMap.at(key) = iValue;
				} else if (stricmp(str.pItem,"enchantchance") || stricmp(str.pItem,"vendorenchantchance")|| stricmp(str.pItem,"bossenchantchance")) {
					enchantMap.at(key) = iValue;
				} else if (stricmp(str.pItem,"scale")|| stricmp(str.pItem,"togglekeycode") || stricmp(str.pItem,"flags") || stricmp(str.pItem,"toggleDuration")) {
					widgetMap.at(key) = iValue;
				} else if (stricmp(str.pItem,"positionx") || stricmp(str.pItem,"positiony")) {
					if (iValue <= 100)
						widgetMap.at(key) = iValue;
				} else if (stricmp(str.pItem,"unbreakablecolor") || stricmp(str.pItem,"breakablecolor")) {
					if (iValue <= 0xFFFFFF)
						widgetMap.at(key) = iValue;
				}
			}
		}
	}
}

void INIFile::SetINIData1(std::list<CSimpleIniA::Entry> *list, const char* section, std::unordered_map<std::string, double> *map, CSimpleIniA *iniSettings) {
	for (CSimpleIniA::Entry str : *list) {
		std::string key(str.pItem);
		ToLower(key);

		if (!key.empty()) {
			std::string value(iniSettings->GetValue(section, str.pItem));
			double dValue = 0.0;
			try {
				dValue = std::stod(value);
			} catch (const std::invalid_argument& e) {
				logger::debug("{} : invalid argument", key);
				logger::debug("   {}", e.what());
			} catch (const std::out_of_range& e) {
				logger::debug("{} : out of range", key);
				logger::debug("   {}", e.what());
			}

			if (stricmp(section, "MaterialMultiplier"))
				KeywordDecoder(map, key, dValue);
			else if (dValue >= 0.0)
				map->at(key) = dValue;
		}
	}
}

void INIFile::SetINIData2(std::list<CSimpleIniA::Entry> *list, std::unordered_set<int> *set, CSimpleIniA *iniBreak) {
	for (CSimpleIniA::Entry strESP : *list) {
		std::string esp(strESP.pItem);
		auto index = RE::TESDataHandler::GetSingleton()->GetModIndex(esp);
		
		if (index.value() != 0xFF) {
			std::list<CSimpleIniA::Entry> entryList;
			iniBreak->GetAllKeys(esp.c_str(), entryList);

			for (CSimpleIniA::Entry str : entryList) {
				std::string value = iniBreak->GetValue(strESP.pItem, str.pItem);
				int iValue = std::atoi(value.c_str());
				if (iValue == 1) {
					uint32_t form = 0;
					try {
						form = std::stoi(str.pItem, nullptr, 0);
					} catch (const std::invalid_argument& e) {
						logger::debug("%s : invalid argument", esp.c_str());
						logger::debug("   %s", e.what());
					} catch (const std::out_of_range& e) {
						logger::debug("%s : out of range", esp.c_str());
						logger::debug("   %s", e.what());
					}

					set->insert((index.value() << 24) + form);
				}
			}
		}
	}
}

void INIFile::SetINIData3(std::list<CSimpleIniA::Entry> *list, const wchar_t* filename, CSimpleIniA *iniEnch) {
	const auto dataHandler = RE::TESDataHandler::GetSingleton();
	for (CSimpleIniA::Entry strESP : *list) {
		std::string esp(strESP.pItem);
		auto index = RE::TESDataHandler::GetSingleton()->GetModIndex(esp);
		
		if (index && index.value() != 0xFF) {
			std::list<CSimpleIniA::Entry> entryList;
			iniEnch->GetAllKeys(esp.c_str(), entryList);

			for (CSimpleIniA::Entry str : entryList) {
				std::vector<std::string> split_value = split(iniEnch->GetValue(strESP.pItem, str.pItem),'|');
				std::string part = split_value.at(0);
				std::string name = split_value.at(1);
				ToLower(part);

				// Look for the item
				uint32_t form = std::stoi(str.pItem, nullptr, 0);
				RE::EnchantmentItem* newItem = dataHandler->LookupForm(RE::FormID(form), esp)->As<RE::EnchantmentItem>();

				// Set our enchantment values
				Enchantments newEnch;
				newEnch.enchantment = newItem;
				newEnch.name = name;

				// Add the enchantment to the correct lists
				if (part.rfind("weapon") != std::string::npos)
					enchantWeapon.push_back(newEnch);
				if (part.rfind("head") != std::string::npos)
					enchantHead.push_back(newEnch);
				if (part.rfind("body") != std::string::npos)
					enchantBody.push_back(newEnch);
				if (part.rfind("hand") != std::string::npos)
					enchantHand.push_back(newEnch);
				if (part.rfind("foot") != std::string::npos)
					enchantFoot.push_back(newEnch);
				if (part.rfind("shield") != std::string::npos)
					enchantShield.push_back(newEnch);
			}
		}
	}
}

void INIFile::SetVendorList() {
	// Get all factions
	RE::BSTArray<RE::TESForm *> factionList = RE::TESDataHandler::GetSingleton()->GetFormArray(RE::FormType::Faction);

	// Loop through all found factions
	for (RE::TESForm * faction : factionList) {
		RE::TESFaction* foundFaction = faction->As<RE::TESFaction>();

		if (foundFaction->IsVendor() && foundFaction->vendorData.merchantContainer)
			if (!(std::find(vendorContainers.begin(), vendorContainers.end(), foundFaction->vendorData.merchantContainer) != vendorContainers.end())) {
				vendorContainers.push_back(foundFaction->vendorData.merchantContainer);
			}
	}

	logger::debug("Vendor Containers Found: {}", vendorContainers.size());
}

void INIFile::KeywordDecoder(std::unordered_map<std::string, double> *map, std::string materialType, double dValue) {

	// Fur, Leather, Iron, Steel, Silver, Elven, Bonemold, Falmer, Dwarven, Glass, Chitin, Orcish, Dragon, Ebony, Stalhrim, Daedric
	if (stricmp(materialType.c_str(),"Daedric")) {
		map->insert(std::make_pair("armormaterialdaedric", dValue));
		map->insert(std::make_pair("weapmaterialdaedric", dValue));

	} else if (stricmp(materialType.c_str(),"Dragon")) {
		map->insert(std::make_pair("dlc1weapmaterialdragonbone", dValue));
		map->insert(std::make_pair("armormaterialdragonscale", dValue));
		map->insert(std::make_pair("armormaterialdragonplate", dValue));

	} else if (stricmp(materialType.c_str(),"Ebony")) {
		map->insert(std::make_pair("weapmaterialebony", dValue));
		map->insert(std::make_pair("armormaterialebony", dValue));

	} else if (stricmp(materialType.c_str(),"Stalhrim")) {
		map->insert(std::make_pair("dlc2weaponmaterialstalhrim", dValue));
		map->insert(std::make_pair("dlc2armormaterialstalhrimheavy", dValue));
		map->insert(std::make_pair("dlc2armormaterialstalhrimlight", dValue));

	} else if (stricmp(materialType.c_str(),"Orcish")) {
		map->insert(std::make_pair("weapmaterialorcish", dValue));
		map->insert(std::make_pair("armormaterialorcish", dValue));

	} else if (stricmp(materialType.c_str(),"Glass")) {
		map->insert(std::make_pair("weapmaterialglass", dValue));
		map->insert(std::make_pair("armormaterialglass", dValue));

	} else if (stricmp(materialType.c_str(),"Dwarven")) {
		map->insert(std::make_pair("weapmaterialdwarven", dValue));
		map->insert(std::make_pair("armormaterialdwarven", dValue));

	} else if (stricmp(materialType.c_str(),"Chitin")) {
		map->insert(std::make_pair("dlc2armormaterialchitinheavy", dValue));
		map->insert(std::make_pair("dlc2armormaterialchitinlight", dValue));
		map->insert(std::make_pair("dlc2armormaterialmoragtong", dValue));


	} else if (stricmp(materialType.c_str(),"Falmer")) {
		map->insert(std::make_pair("weapmaterialfalmer", dValue));
		map->insert(std::make_pair("weapmaterialfalmerhoned", dValue));
		map->insert(std::make_pair("dlc1armormaterielfalmerheavy", dValue));
		map->insert(std::make_pair("dlc1armormaterielfalmerheavyoriginal", dValue));
		map->insert(std::make_pair("dlc1armormaterialfalmerhardened", dValue));

	} else if (stricmp(materialType.c_str(),"Bonemold")) {
		map->insert(std::make_pair("dlc2armormaterialbonemoldheavy", dValue));

	} else if (stricmp(materialType.c_str(),"Elven")) {
		map->insert(std::make_pair("weapmaterialelven", dValue));
		map->insert(std::make_pair("armormaterialelven", dValue));
		map->insert(std::make_pair("armormaterialelvengilded", dValue));

	} else if (stricmp(materialType.c_str(),"Silver")) {
		map->insert(std::make_pair("weapmaterialsilver", dValue));

	} else if (stricmp(materialType.c_str(),"Steel")) {
		map->insert(std::make_pair("weapmaterialsteel", dValue));
		map->insert(std::make_pair("weapmaterialimperial", dValue));
		map->insert(std::make_pair("weapmaterialdraugr", dValue));
		map->insert(std::make_pair("weapmaterialdraugrhoned", dValue));
		map->insert(std::make_pair("dlc2weaponmaterialnordic", dValue));
		map->insert(std::make_pair("armormaterialsteel", dValue));
		map->insert(std::make_pair("armormaterialscaled", dValue));
		map->insert(std::make_pair("armormaterialblades", dValue));
		map->insert(std::make_pair("armormaterialimperialheavy", dValue));
		map->insert(std::make_pair("armormaterialpenitus", dValue));
		map->insert(std::make_pair("dlc2armormaterialnordicheavy", dValue));
		map->insert(std::make_pair("armormaterialsteelplate", dValue));
		map->insert(std::make_pair("dlc1armormaterialdawnguard", dValue));
		map->insert(std::make_pair("dlc1armormaterialhunter", dValue));

	} else if (stricmp(materialType.c_str(),"Iron")) {
		map->insert(std::make_pair("weapmaterialiron", dValue));
		map->insert(std::make_pair("armormaterialiron", dValue));
		map->insert(std::make_pair("armormaterialironbanded", dValue));
		map->insert(std::make_pair("armormaterialimperialstudded", dValue));
		map->insert(std::make_pair("armormaterialstudded", dValue));

	} else if (stricmp(materialType.c_str(),"Leather")) {
		map->insert(std::make_pair("armormaterialleather", dValue));
		map->insert(std::make_pair("armormaterialimperiallight", dValue));
		map->insert(std::make_pair("dlc1armormaterialvampire", dValue));

	} else if (stricmp(materialType.c_str(),"Fur")) {
		map->insert(std::make_pair("armormaterialhide", dValue));
		map->insert(std::make_pair("armormaterialstormcloak", dValue));

	}

}

void INIFile::ShowSettings() {
	logger::debug("Degradation");
	for (auto& map : degradationMap) {
		logger::debug("   {} = {}", map.first, map.second);
	}

	logger::debug("DegradationRate");
	for (auto& map : degradationRateMap) {
		logger::debug("   {} = {}", map.first, map.second);
	}

	logger::debug("BreakChance");
	for (auto& map : breakChanceMap) {
		logger::debug("   {} = {}", map.first, map.second);
	}

	logger::debug("DynamicTemper");
	for (auto& map : temperMap) {
		logger::debug("   {} = {}", map.first, map.second);
	}

	logger::debug("DynamicEnchant");
	for (auto& map : enchantMap) {
		logger::debug("   {} = {}", map.first, map.second);
	}

	logger::debug("Widget");
	for (auto& map : widgetMap) {
		logger::debug("   {} = {}", map.first, map.second);
	}

	logger::debug("Material");
	logger::debug("   Loaded {} Material Keywords", materialMap.size());

	logger::debug("Enchantments");
	logger::debug("   Loaded {} weapon enchantments", enchantWeapon.size());
	logger::debug("   Loaded {} head enchantments", enchantHead.size());
	logger::debug("   Loaded {} body enchantments", enchantBody.size());
	logger::debug("   Loaded {} hand enchantments", enchantHand.size());
	logger::debug("   Loaded {} foot enchantments", enchantFoot.size());
	logger::debug("   Loaded {} shield enchantments", enchantShield.size());
}