#pragma once

#include "SimpleIni.h"
#include <unordered_map>
#include <unordered_set>

struct Enchantments {
	RE::EnchantmentItem* enchantment;
	std::string name;
};

class INIFile
{
public:
	INIFile();
	void Load();

	int GetDegradationSettings(std::string str);
	double GetDegradationRateSettings(std::string str);
	double GetDegradationRateSettings(RE::TESForm* form);
	double GetBreakChanceSettings(std::string str);
	double GetBreakChanceSettings(RE::TESForm* form);
	int GetTemperSettings(std::string str);
	int GetEnchantSettings(std::string str);
	int GetWidgetSettings(std::string str);

	std::vector<Enchantments>* GetEnchantmentList(std::string part);
	int GetEnchantmentSize(std::string part);
	RE::EnchantmentItem* GetEnchantmentForm(std::string part, int index);
	std::string GetEnchantmentName(std::string part, int index);

	bool IsVendorContainer(RE::TESObjectREFR* form);

	bool HasNoBreakForms(int formid);

	float EquipmentHealthThreshold;

private:
	void ToLower(std::string &str);
	bool stricmp(const char* str_1, const char* str_2);
	std::vector<std::string> split(std::string str, char delim);
	double MaterialRate(std::span<RE::BGSKeyword*> keywords);

	void SetVendorList();

	void SetSettings();
	void SetINIData1(std::list<CSimpleIniA::Entry> *list, const char* section, CSimpleIniA *iniSettings);
	void SetINIData1(std::list<CSimpleIniA::Entry> *list, const char* section, std::unordered_map<std::string, double> *map, CSimpleIniA *iniSettings);
	void SetINIData2(std::list<CSimpleIniA::Entry> *list, std::unordered_set<int> *set, CSimpleIniA *iniBreak);
	void SetINIData3(std::list<CSimpleIniA::Entry> *list, const wchar_t* filename, CSimpleIniA *iniEnch);
	void SetINIData4(std::list<CSimpleIniA::Entry> *list, const char* section, std::unordered_map<std::string, double> *map, CSimpleIniA *iniSettings);

	std::string KeywordLookup(uint32_t formid, std::string_view pluginname);
	void KeywordInsert(std::unordered_map<std::string, double> *map, std::string editorID, double dValue);
	void KeywordDecoder(std::unordered_map<std::string, double> *map, std::string materialType, double dValue);

	void ShowSettings();

	// System Settings
	std::unordered_map<std::string, int> degradationMap;
	std::unordered_map<std::string, int> temperMap;
	std::unordered_map<std::string, int> enchantMap;
	std::unordered_map<std::string, int> widgetMap;

	// Degradation Rates
	std::unordered_map<std::string, double> degradationRateMap;
	std::unordered_map<std::string, double> breakChanceMap;
	std::unordered_map<std::string, double> materialMap;

	// Break Forms
	std::unordered_set<int> noBreakForms;

	// Enchantment Lists
	std::vector<Enchantments> enchantWeapon;
	std::vector<Enchantments> enchantBody;
	std::vector<Enchantments> enchantHead;
	std::vector<Enchantments> enchantHand;
	std::vector<Enchantments> enchantFoot;
	std::vector<Enchantments> enchantShield;

	// Vendor Chest
	std::vector<RE::TESObjectREFR*> vendorContainers;
};

extern INIFile ini;