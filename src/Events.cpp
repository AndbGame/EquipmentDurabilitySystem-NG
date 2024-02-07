#undef GetObject

#include "Events.h"
#include "FoundEquipData.h"
#include "Settings.h"
#include "Utility.h"

#include <unordered_set>
#include <set>
#include <array>
#include <random>

class ExtraLocationRefType : public RE::BSExtraData {
public:
	RE::BGSLocationRefType * refType;
};

std::mt19937 mt(std::random_device{}());
std::mutex lock_mt;

std::unordered_set<RE::Actor*> list;
std::mutex lock_list;

bool skip = false;
int UpdateCount = 0;

static bool Probability(int value) {
	if (value == 0)
		return false;

	std::uniform_int_distribution<> score{0, 99};
	std::lock_guard<std::mutex> guard(lock_mt);
	int guess = score(mt);

	if (guess < value)
		return true;

	return false;
}

static bool Probability(double value) {
	if (value <= 0.0)
		return false;

	std::uniform_real_distribution<double> score{0.0, 100.0};
	std::lock_guard<std::mutex> guard(lock_mt);
	double guess = score(mt) ;

	if (guess < value)
		return true;

	return false;
}

static void ShuffleSlots(std::array<RE::BGSBipedObjectForm::BipedObjectSlot, 4> *slots) {
	std::lock_guard<std::mutex> guard(lock_mt);
	std::shuffle(slots->begin(), slots->end(), mt);
}

static void ShuffleSlots(std::vector<RE::BGSBipedObjectForm::BipedObjectSlot>* slots) {
    std::lock_guard<std::mutex> guard(lock_mt);
    std::shuffle(slots->begin(), slots->end(), mt);
}

static double GetRandom(double a, double b) {
	std::uniform_real_distribution<> score(a, b);
	std::lock_guard<std::mutex> guard(lock_mt);
	return score(mt) * 0.0001;
}

static bool AddActor(RE::Actor* actor) {
	std::lock_guard<std::mutex> guard(lock_list);

	auto result = list.insert(actor);
	if (result.second) {
		skip = true;
		return true;
	}

	return false;
}

static void ClearActor() {
	std::lock_guard<std::mutex> guard(lock_list);

	if (skip)
		skip = false;
	else if (!list.empty())
		list.clear();
}

static void RemoveEquipment(FoundEquipData eqD, RE::Actor* actor) {
	auto utility = Utility::GetSingleton();
	if (actor == utility->GetPlayer()) {
		eqD.GenerateName();
		auto msg = std::format("Your {} has broken", eqD.pForm->GetName());
		utility->ShowNotification(msg,false,"VOCShoutImpactDisarm");
	}
	actor->RemoveItem(eqD.pForm->As<RE::TESBoundObject>(), 1, RE::ITEM_REMOVE_REASON::kRemove, eqD.pExtraData, nullptr, 0, 0);
	actor->Update(0);
}

static void TemperDecay(FoundEquipData eqD, RE::Actor* actor, bool powerAttack) {
	auto utility = Utility::GetSingleton();

	if (!eqD.pForm)
		return;

	if (!AddActor(actor))
		return;

	float itemHealthPercent = eqD.GetItemHealthPercent();

	// Check if item will break
	if (itemHealthPercent <= ini.EquipmentHealthThreshold)
	{
		double chance = ini.GetBreakChanceSettings(eqD.pForm);

		if (chance != 0.0 && eqD.CanBreak())
		{
			if (ini.GetDegradationSettings("IncreaseDurability") == 1 && itemHealthPercent > 1.0f)
				chance *= 1.0 - ((itemHealthPercent - 1.0f) / (ini.EquipmentHealthThreshold - 1.0f));

			if (powerAttack)
				chance *= ini.GetBreakChanceSettings("PowerAttackMultiplier");

			if (actor != utility->GetPlayer())
			{
				if (actor->IsPlayerTeammate())
					chance *= ini.GetBreakChanceSettings("FollowerMultiplier");
				else
					chance *= ini.GetBreakChanceSettings("NPCMultiplier");
			}

			if (Probability(chance))
			{
				RemoveEquipment(eqD, actor);
				return;
			}
		}
	}

	if (itemHealthPercent <= 1.0f)
		return;

	double rate = ini.GetDegradationRateSettings(eqD.pForm);
	if (rate == 0.0)
		return;

	// Health Degredation
	if (powerAttack)
		rate *= ini.GetDegradationRateSettings("PowerAttackMultiplier");

	if (actor != utility->GetPlayer())
	{
		if (actor->IsPlayerTeammate() != 0)
			rate *= ini.GetDegradationRateSettings("FollowerMultiplier");
		else
			rate *= ini.GetDegradationRateSettings("NPCMultiplier");
	}

	itemHealthPercent -= GetRandom(rate, std::pow(rate + 1.0, 2.0));

	if (itemHealthPercent < 1.0f)
		itemHealthPercent = 1.0f;

	eqD.SetItemHealthPercent(itemHealthPercent);
}

// On Hit event. Decay player or enemy equipment
class HitEventHandler : public RE::BSTEventSink<RE::TESHitEvent> {
public:
    std::mutex container_mutex;
        
    static HitEventHandler* GetSingleton() {
        static HitEventHandler singleton;
        return &singleton;
    }

    RE::BSEventNotifyControl ProcessEvent(const RE::TESHitEvent* a_event, [[maybe_unused]] RE::BSTEventSource<RE::TESHitEvent>* a_eventSource) override 
	{
		auto utility = Utility::GetSingleton();

		if (a_event->target && a_event->target->formType == RE::FormType::ActorCharacter) {
			RE::Actor* actor = a_event->target->As<RE::Actor>();
			if (actor == utility->GetPlayer() || ini.GetDegradationSettings("OnlyPlayer") == 0) {
				RE::InventoryChanges *exChanges = actor->GetInventoryChanges();
				if (exChanges) {
					RE::TESForm* form = RE::TESForm::LookupByID(a_event->source);

					if (form && ((form->formType == RE::FormType::Weapon && !form->As<RE::TESObjectWEAP>()->IsStaff()) || (form->formType == RE::FormType::Armor && form->As<RE::TESObjectARMO>()->IsShield()))) {
						
						// Decay weapon/shield if it was used or parried with
						if ((a_event->flags & RE::TESHitEvent::Flag::kHitBlocked) != RE::TESHitEvent::Flag::kNone) {
							FoundEquipData eqD_armor = FoundEquipData::FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kShield);
							if (eqD_armor.pForm) {
								TemperDecay(eqD_armor, actor, (a_event->flags & RE::TESHitEvent::Flag::kPowerAttack) != RE::TESHitEvent::Flag::kNone);
							} else{
								RE::TESForm* weap = actor->GetEquippedObject(false);
								if (weap) {
									if (!weap->As<RE::TESObjectWEAP>()->IsBound())
										TemperDecay(FoundEquipData::FindEquippedWeapon(exChanges, false, weap), actor, (a_event->flags & RE::TESHitEvent::Flag::kPowerAttack) != RE::TESHitEvent::Flag::kNone);
								}
							}

						// Decay armor, amrmor slots are shuffled for decay loss
						} else {
                            /*std::array<RE::BGSBipedObjectForm::BipedObjectSlot, 4> slots = { 
                                RE::BGSBipedObjectForm::BipedObjectSlot::kHead, 
                                RE::BGSBipedObjectForm::BipedObjectSlot::kBody, 
                                RE::BGSBipedObjectForm::BipedObjectSlot::kHands, 
                                RE::BGSBipedObjectForm::BipedObjectSlot::kFeet
                            };
							ShuffleSlots(&slots);*/

                            std::vector<RE::BGSBipedObjectForm::BipedObjectSlot> slots = {
                                RE::BGSBipedObjectForm::BipedObjectSlot::kHead,
                                //RE::BGSBipedObjectForm::BipedObjectSlot::kHair,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kBody,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kHands,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kForearms,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kAmulet,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kRing,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kFeet,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kCalves,
                                //RE::BGSBipedObjectForm::BipedObjectSlot::kShield,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kTail,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kLongHair,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kCirclet,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kEars,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kModMouth,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kModNeck,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kModChestPrimary,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kModBack,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kModMisc1,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kModPelvisPrimary,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kDecapitateHead,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kDecapitate,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kModPelvisSecondary,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kModLegRight,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kModLegLeft,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kModFaceJewelry,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kModChestSecondary,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kModShoulder,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kModArmLeft,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kModArmRight,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kModMisc2,
                                RE::BGSBipedObjectForm::BipedObjectSlot::kFX01
                            };
                            ShuffleSlots(&slots);

							for (RE::BGSBipedObjectForm::BipedObjectSlot slot : slots) {
								FoundEquipData eqD_armor = FoundEquipData::FindEquippedArmor(exChanges, slot);
								if (eqD_armor.pForm) {
                                    logger::info("TemperDecay <{:08X}:{}> => <{:08X}:{}> health={}", actor->GetFormID(),
                                                 actor->GetName(), eqD_armor.pForm->GetFormID(),
                                                 eqD_armor.pForm->GetName(), eqD_armor.GetItemHealthPercent());
									TemperDecay(eqD_armor, actor, (a_event->flags & RE::TESHitEvent::Flag::kPowerAttack) != RE::TESHitEvent::Flag::kNone);
									break;
								} else if (slot == RE::BGSBipedObjectForm::BipedObjectSlot::kHead) {
									eqD_armor = FoundEquipData::FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kHair);
									if (eqD_armor.pForm) {
										TemperDecay(eqD_armor, actor, (a_event->flags & RE::TESHitEvent::Flag::kPowerAttack) != RE::TESHitEvent::Flag::kNone);
										break;
									}
								}
							}
						}
					}
				}
			}
		}

		if (a_event->cause && a_event->cause->formType == RE::FormType::ActorCharacter) {
			RE::Actor* actor = a_event->cause->As<RE::Actor>();
			if (actor == utility->GetPlayer() || ini.GetDegradationSettings("OnlyPlayer") == 0) {
				RE::InventoryChanges *exChanges = actor->GetInventoryChanges();
				if (exChanges) {
					RE::TESForm* form = RE::TESForm::LookupByID(a_event->source);
					if (form) {
						if (form->formType == RE::FormType::Weapon) {
							RE::TESObjectWEAP* weap = form->As<RE::TESObjectWEAP>();
							if (!weap->IsStaff() && !weap->IsBound()) {
								if (form == actor->GetEquippedObject(false)) {
									if (form == actor->GetEquippedObject(true)) {
										bool bLeftHandAttack;
										static RE::BSFixedString strLeftHandAttack = "bLeftHandAttack";
										if (actor->GetGraphVariableBool(strLeftHandAttack, bLeftHandAttack) && bLeftHandAttack)
											TemperDecay(FoundEquipData::FindEquippedWeapon(exChanges, true, form), actor, (a_event->flags & RE::TESHitEvent::Flag::kPowerAttack) != RE::TESHitEvent::Flag::kNone);
										else
											TemperDecay(FoundEquipData::FindEquippedWeapon(exChanges, false, form), actor, (a_event->flags & RE::TESHitEvent::Flag::kPowerAttack) != RE::TESHitEvent::Flag::kNone);
									} else {
										TemperDecay(FoundEquipData::FindEquippedWeapon(exChanges, false, form), actor, (a_event->flags & RE::TESHitEvent::Flag::kPowerAttack) != RE::TESHitEvent::Flag::kNone);
									}
								} else if (form == actor->GetEquippedObject(true)) {
									TemperDecay(FoundEquipData::FindEquippedWeapon(exChanges, true, form), actor, (a_event->flags & RE::TESHitEvent::Flag::kPowerAttack) != RE::TESHitEvent::Flag::kNone);
								}
							}
						} else if (form->formType == RE::FormType::Armor) {
							if (form->As<RE::TESObjectARMO>()->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kShield)) {
								TemperDecay(FoundEquipData::FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kShield), actor, (a_event->flags & RE::TESHitEvent::Flag::kPowerAttack) != RE::TESHitEvent::Flag::kNone);
							}
						}
					}
				}
			}
		}

		return RE::BSEventNotifyControl::kContinue;
    }

    static void Register() {
        RE::ScriptEventSourceHolder* eventHolder = RE::ScriptEventSourceHolder::GetSingleton();
        eventHolder->AddEventSink(HitEventHandler::GetSingleton());
    }
};

// Get references within radius of the player and log them
static void GetCloseEquipment(std::set<RE::TESObjectREFR*> *containerlist, std::set<RE::TESObjectREFR*> *equipmentlist) {
	auto utility = Utility::GetSingleton();
	auto TES = RE::TES::GetSingleton();

	if (TES) {
		static double fRadiusSquare = std::pow(7000, 2.0);
		TES->ForEachReferenceInRange(utility->GetPlayer(), fRadiusSquare, [&](RE::TESObjectREFR& b_ref) {
			if (!b_ref.IsDisabled() && !b_ref.IsPlayerRef() && !b_ref.IsPlayer()) {
				RE::TESObjectREFR* newRef = &b_ref;
				if (b_ref.formType == RE::FormType::ActorCharacter || b_ref.GetBaseObject()->formType == RE::FormType::Container) {
					containerlist->insert(newRef);
				} else if (b_ref.GetBaseObject()->IsArmor()|| b_ref.GetBaseObject()->IsWeapon()) {
					equipmentlist->insert(newRef);
				}
			}
			return RE::BSContainer::ForEachResult::kContinue;
		});
	}
}

// Apply a temper on items that meet the requirements and hit the temper chance
static void DynamicTemperEnchant() {
	auto utility = Utility::GetSingleton();
	auto TES = RE::TES::GetSingleton();

	// Get nearby Equipment and Containers
	std::set<RE::TESObjectREFR*> containerlist;
	std::set<RE::TESObjectREFR*> equipmentlist;
	GetCloseEquipment(&containerlist, &equipmentlist);

	// Loop through equipment list
	if (!equipmentlist.empty()) {
		for (auto& ref : equipmentlist) {
			if (!ref || !ref->GetBaseObject())
				continue;

			// Get Equipment
			FoundEquipData eqD(ref->GetBaseObject());

			// Process record only if Extra Data doesnt have additional health
			if (!ref->extraList.HasType(RE::ExtraDataType::kHealth) && eqD.CanTemper()) {

				// Set level variable
				int level = 0;
				RE::TESForm* owner = ref->GetOwner();
				if (owner&& !owner->IsPlayer() && !owner->IsPlayerRef())
					level = utility->GetPlayer()->GetLevel();

				// Set extra data
				eqD.pExtraData = &ref->extraList;

				// Temper
				if (ini.GetTemperSettings("DisableDynamicTemper") == 0 && eqD.CanBreak()) {
					if (level != 0 && Probability(ini.GetTemperSettings("TemperChance")))
						eqD.SetItemHealthPercent(GetRandom(10001.0, 10001.0 + (level + 10) * 100));
				}

				// Enchant
				if (ini.GetEnchantSettings("DisableDynamicEnchant") == 0 && !eqD.IsEnchanted()) {
					if (level != 0 && Probability(ini.GetEnchantSettings("EnchantChance")))
						eqD.SetItemEnchantment(level);
				}

				// Temper the item at normal health so we dont reprocess
				if (!eqD.IsTempered())
					eqD.SetItemHealthPercent(1.0f);
			
			}
		}
	}

	// Loop through container list
	if (!containerlist.empty()) {
		for (auto& ref : containerlist) {
			if (!ref || ref->IsPlayerRef() || ref->IsPlayer())
				continue;

			// Get Container Inventory Changes and the owner of the container
			RE::InventoryChanges *exChanges =  ref->GetInventoryChanges();

			// If there are no changes, or not list, exit
			if (!exChanges || !exChanges->entryList)
				continue;

			// Loop through all items in list
			for (const auto& pEntry : *exChanges->entryList) {
				if (!pEntry || !pEntry->extraLists)
					continue;

				FoundEquipData eqD(pEntry->GetObject());

				// If item cannot be tempered
				if (!eqD.CanTemper())
					continue;

				// Find health data on item
				for (const auto& pExtraDataList : *pEntry->extraLists) {
					if (!pExtraDataList)
						continue;

					// Tempered Health
					if (!pExtraDataList->HasType(RE::ExtraDataType::kHealth))
						eqD.pExtraData = pExtraDataList;

					// Enchantment
					if (!pExtraDataList->HasType(RE::ExtraDataType::kEnchantment))
						eqD.pExtraData = pExtraDataList;

					break;
				}

				// If extra data of health was found, exit
				if (eqD.pExtraData && eqD.pExtraData->HasType(RE::ExtraDataType::kHealth))
					continue;

				// Get either the actors level, or use the players level to determine strength
				int level = 0;
				if (ref->formType == RE::FormType::ActorCharacter) {
					RE::Actor* actor = ref->As<RE::Actor>();
					if (actor != utility->GetPlayer() && !actor->IsInFaction(utility->factionFollower1) && !actor->IsInFaction(utility->factionFollower2))
						level = actor->GetLevel();
				} else
					level = utility->GetPlayer()->GetLevel();

				// If the item can break, and player/character level is not 0. Set health regardless so item wont be processed again
				if (level != 0 && eqD.CanBreak()) {

					// Get Location Type
					RE::ExtraLocationRefType * xRefType = static_cast<RE::ExtraLocationRefType*>(ref->extraList.GetByType(RE::ExtraDataType::kLocationRefType));

					// Temper
					if (ini.GetTemperSettings("DisableDynamicTemper") == 0) {
						int chanceTemper = ini.GetTemperSettings("TemperChance");
						if (ref->GetBaseObject()->formType == RE::FormType::Container && ini.IsVendorContainer(ref))
							chanceTemper = ini.GetEnchantSettings("VendorTemperChance");
						else if (xRefType && (xRefType->locRefType == utility->locationBoss || xRefType->locRefType == utility->locationBossContainer))
							chanceTemper = ini.GetTemperSettings("BossTemperChance");

						if (Probability(chanceTemper))
							eqD.SetItemHealthPercent(GetRandom(10001.0, 10001.0 + (level + 10) * 100));
					}

					// Enchant
					if (!eqD.IsEnchanted() && ini.GetEnchantSettings("DisableDynamicEnchant") == 0) {
						int chanceEnchant = ini.GetEnchantSettings("EnchantChance");
						if (ref->GetBaseObject()->formType == RE::FormType::Container && ini.IsVendorContainer(ref))
							chanceEnchant = ini.GetEnchantSettings("VendorEnchantChance");
						else if (xRefType && (xRefType->locRefType == utility->locationBoss || xRefType->locRefType == utility->locationBossContainer))
							chanceEnchant = ini.GetEnchantSettings("BossEnchantChance");

						if (Probability(chanceEnchant))
							eqD.SetItemEnchantment(level);
					}

					// Temper the item at normal health so we dont reprocess
					if (!eqD.IsTempered())
						eqD.SetItemHealthPercent(1.0f);
					
				} else
					eqD.SetItemHealthPercent(1.0f);
			}
		}
	}
}

// On Update event to run Dynamic Temper
static std::int32_t OnUpdate() {
	if (!Utility::GetUI()->GameIsPaused()) {

		ClearActor();

		if (g_deltaTime > 0) {
			lastTime += g_deltaTime;
			if (lastTime >= 1.0f) {
				if (ini.GetTemperSettings("DisableDynamicTemper") == 0 || ini.GetEnchantSettings("DisableDynamicEnchant") == 0)
					DynamicTemperEnchant();
				lastTime = 0;
			}
		}
	}

	return _OnUpdate();
}

namespace Events {
	inline static REL::Relocation<std::uintptr_t> On_Update_Hook{ REL::RelocationID(35565, 36564), REL::Relocate(0x748, 0xC26) };

	void Init(void)
	{
		if (ini.GetDegradationSettings("DisableDegradation") == 0) {
			HitEventHandler::Register();
		}

		if (ini.GetTemperSettings("DisableDynamicTemper") == 0 || ini.GetEnchantSettings("DisableDynamicEnchant") == 0) {
			auto& trampoline = SKSE::GetTrampoline();
			_OnUpdate = trampoline.write_call<5>(On_Update_Hook.address(), OnUpdate);
			logger::info("Installed update hook");
		}
	}
}