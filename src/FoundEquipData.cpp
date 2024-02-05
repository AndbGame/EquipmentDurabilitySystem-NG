#undef GetObject
#include "FoundEquipData.h"
#include "Settings.h"
#include "Utility.h"

void FoundEquipData::GenerateName() {
	if (!pForm || !pExtraData)
		return;

	RE::InventoryEntryData NewEntry = RE::InventoryEntryData(pForm->As<RE::TESBoundObject>(), pExtraData->GetCount());
	NewEntry.AddExtraList(pExtraData);

	// Set name to unarmed, or the object fine
	if (pForm->formID == 0x0001F4)
		name = "Unarmed";
	else
		name = std::string(NewEntry.GetDisplayName());

	// Add the poison if applicable
	if (pForm->IsWeapon() && ini.GetWidgetSettings("HidePoisonName") == 0 && pExtraData->HasType(RE::ExtraDataType::kPoison)) {
		RE::ExtraPoison* xPoison = static_cast<RE::ExtraPoison*>(pExtraData->GetByType(RE::ExtraDataType::kPoison));
		if (xPoison && xPoison->poison)
			name += " " + std::string(xPoison->poison->GetFullName()) + " (" + std::to_string(xPoison->count) + ")";
	}
	NewEntry.extraLists->clear();
}

float FoundEquipData::GetItemHealthPercent() {
	if (!pExtraData)
		return 1.0f;

	RE::ExtraHealth* xHealth = static_cast<RE::ExtraHealth*>(pExtraData->GetByType(RE::ExtraDataType::kHealth));
	if (xHealth)
		return xHealth->health;

	return 1.0f;
}

void FoundEquipData::SetItemHealthPercent(float value) {
	if (!pExtraData)
		return;

	RE::ExtraHealth* xHealth = static_cast<RE::ExtraHealth*>(pExtraData->GetByType(RE::ExtraDataType::kHealth));
	if (xHealth) {
		xHealth->health = value;
	} else if (pExtraData) {
		RE::ExtraHealth* newHealth = static_cast<RE::ExtraHealth*>(RE::ExtraHealth::Create(sizeof(RE::ExtraHealth), RE::VTABLE_ExtraHealth[0].address()));
		newHealth->health = value;
		pExtraData->Add(newHealth);
	}
}

int FoundEquipData::GetEnchantmentListSize() {
	if (!pForm)
		return 0;

	// Return the size of the enchantment list based on the object
	if (pForm->IsWeapon())
		return ini.GetEnchantmentSize("weapon");
	else if (pForm->IsArmor()) {
		RE::TESObjectARMO *armor = pForm->As<RE::TESObjectARMO>();
		if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kBody) || armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kTail))
			return ini.GetEnchantmentSize("body");
		else if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHead) || armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHair) || armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kLongHair))
			return ini.GetEnchantmentSize("head");
		else if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHands) || armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kForearms))
			return ini.GetEnchantmentSize("hand");
		else if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kFeet) || armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kCalves))
			return ini.GetEnchantmentSize("foot");
		else if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kShield))
			return ini.GetEnchantmentSize("shield");
	}

	return 0;
}

int FoundEquipData::GetRandom(int a, int b) {
	std::mt19937 mt(std::random_device{}());
	std::mutex lock_mt;

	std::uniform_real_distribution<> score(a, b);
	std::lock_guard<std::mutex> guard(lock_mt);
	return score(mt);
}

void FoundEquipData::SetItemEnchantment(int level) {
	if (!pExtraData)
		return;

	// Set enchantment value to the nearest 100th
	int value = GetRandom(100, 500);
	int newValue = value;
	int remainder = value % 100;
	if (remainder >= 50)
		newValue = value + 100 - remainder;
	else if (remainder != 0)
		newValue = value - remainder;

	int prevSize = ini.GetEnchantmentSize(GetType());
	std::vector<Enchantments>* tempEnch = ini.GetEnchantmentList(GetType());
	std::vector<Enchantments> newEnch;
	Enchantments selectEnch;

	// Set Magnitude threshold
	float magMax = 70.0;
	if (level <= 8)
		magMax = 12.5;
	else if (level <= 16)
		magMax = 15.0;
	else if (level <= 24)
		magMax = 20.0;
	else if (level <= 32)
		magMax = 30.0;
	else if (level <= 40)
		magMax = 45.0;

	// Copy values within the level range
	std::copy_if(tempEnch->begin(), tempEnch->end(), std::back_inserter(newEnch), [magMax](Enchantments i) {return i.enchantment->effects[0]->GetMagnitude() <= magMax; });
	
	// Get the enchantment, return if no enchantments found
	int maxSize = newEnch.size();
	if (maxSize > 0)
		selectEnch = newEnch.at(GetRandom(0,newEnch.size()-1));
	else
		return;
	
	// Looks for enchantment data or create it
	RE::ExtraEnchantment* xEnchantment = static_cast<RE::ExtraEnchantment*>(pExtraData->GetByType(RE::ExtraDataType::kEnchantment));
	if (xEnchantment) {
		xEnchantment->enchantment = selectEnch.enchantment;
		xEnchantment->charge = newValue;
	} else {
		RE::ExtraEnchantment* newEnchantment = static_cast<RE::ExtraEnchantment*>(RE::ExtraEnchantment::Create(sizeof(RE::ExtraEnchantment), RE::VTABLE_ExtraEnchantment[0].address()));
		newEnchantment->enchantment = selectEnch.enchantment;
		newEnchantment->charge = newValue;
		pExtraData->Add(newEnchantment);
	}

	// Look for name data or create it
	name = std::string(pForm->GetName()) + " " + selectEnch.name;
	RE::ExtraTextDisplayData* xTextData = static_cast<RE::ExtraTextDisplayData*>(pExtraData->GetByType(RE::ExtraDataType::kTextDisplayData));
	if (xTextData) {
		xTextData->SetName(name.c_str());
	} else {
		RE::ExtraTextDisplayData* newTextdata = static_cast<RE::ExtraTextDisplayData*>(RE::ExtraTextDisplayData::Create(sizeof(RE::ExtraTextDisplayData), RE::VTABLE_ExtraTextDisplayData[0].address()));
		newTextdata->SetName(name.c_str());
		pExtraData->Add(newTextdata);
	}
}

std::string FoundEquipData::GetType() {
	if (pForm->IsWeapon()) {
		return "weapon";
	} else if (pForm->IsArmor()) {
		RE::TESObjectARMO *armor = pForm->As<RE::TESObjectARMO>();
		if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kBody) || armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kTail))
			return "body";
		else if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHead) || armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHair) || armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kLongHair))
			return "head";
		else if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHands) || armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kForearms))
			return "hand";
		else if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kFeet) || armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kCalves))
			return "foot";
		else if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kShield))
			return "shield";
	}

	return "none";
}

bool FoundEquipData::IsTempered() {
	if (!pExtraData)
		return false;

	RE::ExtraHealth* xHealth = static_cast<RE::ExtraHealth*>(pExtraData->GetByType(RE::ExtraDataType::kHealth));
	if (xHealth)
		return true;

	return false;
}

bool FoundEquipData::IsEnchanted() {
	if (!pExtraData)
		return false;

	// Is the item a weapon and enchanted
	if (pForm && pForm->IsWeapon() && pForm->As<RE::TESObjectWEAP>()->formEnchanting)
		return true;

	// Is the item armor and enchanted
	if (pForm && pForm->IsArmor() && pForm->As<RE::TESObjectARMO>()->formEnchanting)
		return true;

	// Is there already enchanted data
	if(pExtraData->HasType(RE::ExtraDataType::kEnchantment))
		return true;

	// We can enchant
	return false;
}

bool FoundEquipData::CanBreak() {
	if (!pForm || !pExtraData)
		return false;

	// Do not break items that you cannot disenchant, user preference
	auto utility = Utility::GetSingleton();
	if (ini.GetDegradationSettings("NoBreakMagicDisallowEnchanting") == 1) {
		if (pForm->IsWeapon() && pForm->As<RE::TESObjectWEAP>()->HasKeyword(utility->keywordMagicDisallow))
			return false;
		else if (pForm->IsArmor() && pForm->As<RE::TESObjectARMO>()->HasKeyword(utility->keywordMagicDisallow))
			return false;
	}

	// Unarmed
	if (pForm->formID == 0x0001F4)
		return false;

	// Compare to the break form list
	if (ini.HasNoBreakForms(pForm->formID))
		return false;

	// Dont break quest items
	RE::InventoryEntryData newData(pForm->As<RE::TESBoundObject>(), 1);
	if (newData.IsQuestObject())
		return false;

	return true;
}

bool FoundEquipData::CanTemper() {
	if (!pForm)
		return false;

	auto utility = Utility::GetSingleton();
	if (pForm->IsWeapon()) {
		// Form cannot be a staff or a bound weapon
		RE::TESObjectWEAP* weap = pForm->As<RE::TESObjectWEAP>();
		if (!weap->IsStaff() && !weap->IsBound())
			return true;
	} else if (pForm->IsArmor()) {
		// Armor has to bip one of the given armor slots
		RE::TESObjectARMO *armo = pForm->As<RE::TESObjectARMO>();
		if (armo->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHead) || armo->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHair) 
			|| armo->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kBody) || armo->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHands) 
			|| armo->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kFeet) || armo->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kShield))
		{
			if (!armo->HasKeyword(utility->keywordClothing) && !armo->IsClothing())
				return true;
		}
	}

	return false;
}

FoundEquipData FoundEquipData::FindEquippedWeapon(RE::InventoryChanges *exChanges, bool abLeftHand, RE::TESForm* form) {
	FoundEquipData equipData;

	if (exChanges->entryList) {
		for (const auto& pEntry : *exChanges->entryList) {
			if (!pEntry || pEntry->GetObject() != form || !pEntry->extraLists)
				continue;

			for (const auto& pExtraDataList : *pEntry->extraLists) {
				if (pExtraDataList) {
					if ((!abLeftHand && pExtraDataList->HasType(RE::ExtraDataType::kWorn)) || (abLeftHand && pExtraDataList->HasType(RE::ExtraDataType::kWornLeft))) {
						equipData.pForm = pEntry->GetObject();
						equipData.pExtraData = pExtraDataList;

						return equipData;
					}
				}
			}
		}
	}

	return equipData;
}

FoundEquipData FoundEquipData::FindEquippedArmor(RE::InventoryChanges *exChanges, RE::BGSBipedObjectForm::BipedObjectSlot slotMask) {
	FoundEquipData equipData;

	if (exChanges->entryList) {
		for (const auto& pEntry : *exChanges->entryList) {
			if (!pEntry || !pEntry->GetObject()->IsArmor() || !pEntry->GetObject()->As<RE::TESObjectARMO>()->HasPartOf(slotMask) || !pEntry->extraLists)
				continue;

			for (const auto& pExtraDataList : *pEntry->extraLists) {
				if (pExtraDataList) {
					if (pExtraDataList->HasType(RE::ExtraDataType::kWorn) || pExtraDataList->HasType(RE::ExtraDataType::kWornLeft)) {
						equipData.pForm = pEntry->GetObject();
						equipData.pExtraData = pExtraDataList;

						return equipData;
					}
				}
			}
		}
	}

	return equipData;
}