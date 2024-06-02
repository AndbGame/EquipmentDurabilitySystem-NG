#include "DurabilityMenu.h"
#include "FoundEquipData.h"
#include "Settings.h"
#include "Utility.h"

#include <vector>

enum Icon
{
	kIcon_None,
	kIcon_DefaultWeapon,
	kIcon_WeaponSword,
	kIcon_WeaponGreatSword,
	kIcon_WeaponDaedra,
	kIcon_WeaponDagger,
	kIcon_WeaponWarAxe,
	kIcon_WeaponBattleAxe,
	kIcon_WeaponMace,
	kIcon_WeaponHammer,
	kIcon_WeaponStaff,
	kIcon_WeaponBow,
	kIcon_WeaponArrow,
	kIcon_WeaponPickAxe,
	kIcon_WeaponWoodAxe,
	kIcon_WeaponCrossbow,
	kIcon_WeaponBolt,

	kIcon_DefaultArmor,

	kIcon_LightArmorBody,
	kIcon_LightArmorHead,
	kIcon_LightArmorHands,
	kIcon_LightArmorForearms,
	kIcon_LightArmorFeet,
	kIcon_LightArmorCalves,
	kIcon_LightArmorShield,
	kIcon_LightArmorMask,

	kIcon_ArmorBody,
	kIcon_ArmorHead,
	kIcon_ArmorHands,
	kIcon_ArmorForearms,
	kIcon_ArmorFeet,
	kIcon_ArmorCalves,
	kIcon_ArmorShield,
	kIcon_ArmorMask,
	kIcon_ArmorBracer,
	kIcon_ArmorDaedra,

	kIcon_ClothingBody,
	kIcon_ClothingRobe,
	kIcon_ClothingHead,
	kIcon_ClothingPants,
	kIcon_ClothingHands,
	kIcon_ClothingForearms,
	kIcon_ClothingFeet,
	kIcon_ClothingCalves,
	kIcon_ClothingShoes,
	kIcon_ClothingShield,
	kIcon_ClothingMask,

	kIcon_ArmorAmulet,
	kIcon_ArmorRing,
	kIcon_Circlet,

	kIcon_DefaultScroll,

	kIcon_DefaultBook,
	kIcon_BookRead,
	kIcon_BookTome,
	kIcon_TomeRead,
	kIcon_BookJournal,
	kIcon_BookNote,
	kIcon_BookMap,

	kIcon_DefaultFood,
	kIcon_FoodWine,
	kIcon_FoodBeer,

	kIcon_DefaultIngredient,

	kIcon_DefaultKey,
	kIcon_KeyHouse,

	kIcon_DefaultPotion,
	kIcon_PotionHealth,
	kIcon_PotionStam,
	kIcon_PotionMagic,
	kIcon_PotionPoison,
	kIcon_PotionFrost,
	kIcon_PotionFire,
	kIcon_PotionShock,

	kIcon_DefaultMisc,
	kIcon_MiscArtifact,
	kIcon_MiscClutter,
	kIcon_MiscLockPick,
	kIcon_MiscSoulGem,

	kIcon_SoulGemEmpty,
	kIcon_SoulGemPartial,
	kIcon_SoulGemFull,
	kIcon_SoulGemGrandEmpty,
	kIcon_SoulGemGrandPartial,
	kIcon_SoulGemGrandFull,
	kIcon_SoulGemAzura,

	kIcon_MiscGem,
	kIcon_MiscOre,
	kIcon_MiscIngot,
	kIcon_MiscHide,
	kIcon_MiscStrips,
	kIcon_MiscLeather,
	kIcon_MiscWood,
	kIcon_MiscRemains,
	kIcon_MiscTrollSkull,
	kIcon_MiscTorch,
	kIcon_MiscGoldSack,
	kIcon_MiscGold,
	kIcon_MiscDragonClaw,

	kIcon_Default_Alteration,
	kIcon_Default_Illusion,
	kIcon_Default_Destruction,
	kIcon_Default_Conjuration,
	kIcon_Default_Restoration,
	kIcon_Default_Shout,
	kIcon_Default_Power,
	kIcon_Default_Effect,
	kIcon_Magic_Fire,
	kIcon_Magic_Frost,
	kIcon_Magic_Shock
};

static const char * strIcons[] = {
	"none",					// 00
	"default_weapon",
	"weapon_sword",
	"weapon_greatsword",
	"weapon_daedra",
	"weapon_dagger",
	"weapon_waraxe",
	"weapon_battleaxe",
	"weapon_mace",
	"weapon_hammer",
	"weapon_staff",			// 10
	"weapon_bow",
	"weapon_arrow",
	"weapon_pickaxe",
	"weapon_woodaxe",
	"weapon_crossbow",
	"weapon_bolt",
	"default_armor",
	"lightarmor_body",
	"lightarmor_head",
	"lightarmor_hands",		// 20
	"lightarmor_forearms",
	"lightarmor_feet",
	"lightarmor_calves",
	"lightarmor_shield",
	"lightarmor_mask",
	"armor_body",
	"armor_head",
	"armor_hands",
	"armor_forearms",
	"armor_feet",			// 30
	"armor_calves",
	"armor_shield",
	"armor_mask",
	"armor_bracer",
	"armor_daedra",
	"clothing_body",
	"clothing_robe",
	"clothing_head",
	"clothing_pants",
	"clothing_hands",		// 40
	"clothing_forearms",
	"clothing_feet",
	"clothing_calves",
	"clothing_shoes",
	"clothing_shield",
	"clothing_mask",
	"armor_amulet",
	"armor_ring",
	"armor_circlet",
	"default_scroll",		// 50
	"default_book",
	"default_book_read",
	"book_tome",
	"book_tome_read",
	"book_journal",
	"book_note",
	"book_map",
	"default_food",
	"food_wine",
	"food_beer",			// 60
	"default_ingredient",
	"default_key",
	"key_house",
	"default_potion",
	"potion_health",
	"potion_stam",
	"potion_magic",
	"potion_poison",
	"potion_frost",
	"potion_fire",			// 70
	"potion_shock",
	"default_misc",
	"misc_artifact",
	"misc_clutter",
	"misc_lockpick",
	"misc_soulgem",
	"soulgem_empty",
	"soulgem_partial",
	"soulgem_full",
	"soulgem_grandempty",	// 80
	"soulgem_grandpartial",
	"soulgem_grandfull",
	"soulgem_azura",
	"misc_gem",
	"misc_ore",
	"misc_ingot",
	"misc_hide",
	"misc_strips",
	"misc_leather",
	"misc_wood",			// 90
	"misc_remains",
	"misc_trollskull",
	"misc_torch",
	"misc_goldsack",
	"misc_gold",
	"misc_dragonclaw",
	"default_alteration",
	"default_illusion",
	"default_destruction",
	"default_conjuration",	// 100
	"default_restoration",
	"default_shout",
	"default_power",
	"default_effect",
	"magic_fire",
	"magic_frost",
	"magic_shock"
};

// This event sink will create the widget, gets re-ran on game load
class AutoOpenDurabilityMenuSink : public RE::BSTEventSink<RE::MenuOpenCloseEvent> {
public:
	static AutoOpenDurabilityMenuSink* GetSingleton() {
        static AutoOpenDurabilityMenuSink singleton;
        return &singleton;
    }

	RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent *a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent> *a_eventSource) {
		RE::UI *ui = RE::UI::GetSingleton();
		RE::InterfaceStrings *holder = RE::InterfaceStrings::GetSingleton();

		if (a_event->menuName == holder->loadingMenu && !a_event->opening && !ui->IsMenuOpen(holder->mainMenu))
		{
			DurabilityMenu *DurabilityMenu = DurabilityMenu::GetSingleton();
			if (!DurabilityMenu)
			{
				RE::UIMessageQueue *ui = RE::UIMessageQueue::GetSingleton();
				ui->AddMessage("Durability Menu", RE::UI_MESSAGE_TYPE::kShow, nullptr);
			}

			ui->RemoveEventSink(this);
		}

		return RE::BSEventNotifyControl::kContinue;
	}

	static void Register()
	{
		RE::UI *eventHolder = RE::UI::GetSingleton();
        eventHolder->AddEventSink(AutoOpenDurabilityMenuSink::GetSingleton());
	}
};

// Had to move the input handler out of the class to get it to work properly
class InputEventHandler : public RE::BSTEventSink<RE::InputEvent*> {
	public:
    static InputEventHandler* GetSingleton() {
        static InputEventHandler singleton;
        return &singleton;
    }

	RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event,RE::BSTEventSource<RE::InputEvent*>* a_eventSource) {
		if (a_event) {
			auto utility = Utility::GetSingleton();
			const auto dura = DurabilityMenu::GetSingleton();

			if (utility->PlayerNotInMenu()) {
                const auto controlMap = RE::ControlMap::GetSingleton();
                const auto playerCharacter = RE::PlayerCharacter::GetSingleton();
                const auto playerControls = RE::PlayerControls::GetSingleton();

				if (controlMap && playerCharacter && playerControls) {
					for (auto event = *a_event; event; event = event->next) {
						if (event->eventType == RE::INPUT_EVENT_TYPE::kButton) {
							const auto button = static_cast<RE::ButtonEvent*>(event);
							if (!button || (button->IsPressed() && !button->IsDown())) continue;

							auto device = button->device.get();
							auto scan_code = button->GetIDCode();

							if (device == RE::INPUT_DEVICE::kMouse) {
								scan_code += 257;
							} else if (device == RE::INPUT_DEVICE::kGamepad) {
								RE::BSWin32GamepadDevice::Key gamepadKey =
									static_cast<RE::BSWin32GamepadDevice::Key>(scan_code);
								switch (gamepadKey) {
									case RE::BSWin32GamepadDevice::Key::kUp:
										scan_code = 266;
										break;
									case RE::BSWin32GamepadDevice::Key::kDown:
										scan_code = 267;
										break;
									case RE::BSWin32GamepadDevice::Key::kLeft:
										scan_code = 268;
										break;
									case RE::BSWin32GamepadDevice::Key::kRight:
										scan_code = 269;
										break;
									case RE::BSWin32GamepadDevice::Key::kStart:
										scan_code = 270;
										break;
									case RE::BSWin32GamepadDevice::Key::kBack:
										scan_code = 271;
										break;
									case RE::BSWin32GamepadDevice::Key::kLeftThumb:
										scan_code = 272;
										break;
									case RE::BSWin32GamepadDevice::Key::kRightThumb:
										scan_code = 273;
										break;
									case RE::BSWin32GamepadDevice::Key::kLeftShoulder:
										scan_code = 274;
										break;
									case RE::BSWin32GamepadDevice::Key::kRightShoulder:
										scan_code = 275;
										break;
									case RE::BSWin32GamepadDevice::Key::kA:
										scan_code = 276;
										break;
									case RE::BSWin32GamepadDevice::Key::kB:
										scan_code = 277;
										break;
									case RE::BSWin32GamepadDevice::Key::kX:
										scan_code = 278;
										break;
									case RE::BSWin32GamepadDevice::Key::kY:
										scan_code = 279;
										break;
									case RE::BSWin32GamepadDevice::Key::kLeftTrigger:
										scan_code = 280;
										break;
									case RE::BSWin32GamepadDevice::Key::kRightTrigger:
										scan_code = 281;
										break;
									default:
										scan_code = static_cast<uint32_t>(-1);
										break;
								}
							}

							if ((device == RE::INPUT_DEVICE::kKeyboard || device == RE::INPUT_DEVICE::kGamepad) && !button->IsUp()) {
								if (scan_code == ini.GetWidgetSettings("ToggleKeyCode")) {
									dura->hotkeyActivation();
								}
							}


						}
					}
				}
			}
		}

		return RE::BSEventNotifyControl::kContinue;
	}

	static void Register() {
        RE::BSInputDeviceManager* inputDeviceManager = RE::BSInputDeviceManager::GetSingleton();
        inputDeviceManager->AddEventSink(InputEventHandler::GetSingleton());
    }

	static void Unregister() {
        RE::BSInputDeviceManager* inputDeviceManager = RE::BSInputDeviceManager::GetSingleton();
        inputDeviceManager->RemoveEventSink(InputEventHandler::GetSingleton());
    }
};

DurabilityMenu * DurabilityMenu::ms_pSingleton = nullptr;

DurabilityMenu::DurabilityMenu() : updateCount(0), visible(true) {
	// Set up the scaleform loader
	const char swfName[] = "DurabilityMenu";
	RE::BSScaleformManager *sm = RE::BSScaleformManager::GetSingleton();

	// Load the moive and set the flags
	if (sm->LoadMovie(this, uiMovie, swfName, RE::GFxMovieView::ScaleModeType::kNoScale, 0))
	{
		// Load the SWF file with the medium depth priority
		logger::debug("loaded Interface - {}.swf", swfName);
		depthPriority = 0;

		// Dunno how to translate this flag... guessing from 0x10802
		menuFlags.set(RE::UI_MENU_FLAGS::kAllowSaving);
		menuFlags.set(RE::UI_MENU_FLAGS::kRequiresUpdate);
		
		// Allow keyboard input while menu is open
		inputContext = Context::kNone;

		// disable mouse input
		uiMovie->SetMouseCursorCount(0);
	}
}

DurabilityMenu::~DurabilityMenu() {
}

bool DurabilityMenu::Register() {
	RE::UI *ui = RE::UI::GetSingleton();
	if (!ui)
		return false;

	// Register the drability menu
	ui->Register("Durability Menu", []() -> RE::IMenu * { return new DurabilityMenu; });
	AutoOpenDurabilityMenuSink::Register();

	return true;
}

void DurabilityMenu::hotkeyActivation() {

	if (!skseActivated && ini.GetWidgetSettings("ToggleDuration") > 0.0 && !visible) {
		_logged = chrono_clock::now();
		hotkeyActivated = true;
		visible = true;
	} else if (ini.GetWidgetSettings("ToggleDuration") == 0.0 && !visible) {
		visible = true;
	} else if (!skseActivated && visible) {
		visible = false;
	}
}

// This event will open/close the widget depending on what other menus are open
DurabilityMenu::EventResult DurabilityMenu::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource) {
	if (uiMovie) {
		auto utility = Utility::GetSingleton();
		RE::UI *ui = RE::UI::GetSingleton();

		if (a_event->opening && uiMovie->GetVisible()) {
			RE::GPtr<RE::IMenu> menu = ui->GetMenu(a_event->menuName);
			if (menu && (menu->menuFlags.any(RE::UI_MENU_FLAGS::kDontHideCursorWhenTopmost) || menu->menuFlags.any(RE::UI_MENU_FLAGS::kPausesGame)))
				uiMovie->SetVisible(false);
			else if (utility->IsSystemMenu(a_event->menuName))
				uiMovie->SetVisible(false);
		} else if (!uiMovie->GetVisible()) {
			if (ui->numPausesGame == 0 && ui->numDontHideCursorWhenTopmost == 0)
				uiMovie->SetVisible(true);
			else if (!utility->IsSystemMenu(a_event->menuName) && !utility->IsSystemMenuOpen(a_event->menuName))
				uiMovie->SetVisible(true);
		}
	}
	
	return RE::BSEventNotifyControl::kContinue;
}

// This event captures SKSE actions for Draw End an Sheathe End
DurabilityMenu::EventResult DurabilityMenu::ProcessEvent(const SKSE::ActionEvent *a_event, RE::BSTEventSource<SKSE::ActionEvent> *a_eventSource) {
	auto utility = Utility::GetSingleton();

	if (a_event->actor == utility->GetPlayer()) {
		if (a_event->type == SKSE::ActionEvent::Type::kEndDraw) {
			visible = true;
			skseActivated = true;
			hotkeyActivated = false;
		} else if (a_event->type == SKSE::ActionEvent::Type::kEndSheathe) {
			visible = false;
			skseActivated = false;
		}
	}

	return RE::BSEventNotifyControl::kContinue;
}

DurabilityMenu::UI_MESSAGE_RESULTS DurabilityMenu::ProcessMessage(RE::UIMessage& a_message)
{
	RE::UI_MESSAGE_RESULTS result = RE::UI_MESSAGE_RESULTS::kPassOn;

	if (uiMovie)
	{
		if (a_message.type == RE::UI_MESSAGE_TYPE::kShow) {
			OnMenuOpen();
			result = RE::UI_MESSAGE_RESULTS::kHandled;
		} else if (a_message.type == RE::UI_MESSAGE_TYPE::kHide) {
			OnMenuClose();
			result = RE::UI_MESSAGE_RESULTS::kHandled;
		}
	}

	return result;
}

// Update statement, time might be used later
void DurabilityMenu::AdvanceMovie(float a_interval, std::uint32_t a_currentTime)
{
	// Hotkey Display Duration
	if (hotkeyActivated) {
		_current = chrono_clock::now();
		auto fsec = (_current - _logged) / 1000000000.f;
		float deltaTime = static_cast<float>(fsec.count());

		if (deltaTime >= ini.GetWidgetSettings("ToggleDuration")) {
			hotkeyActivated = false;
			visible = false;
		}
	}

	//
	if (++updateCount > 10) {
		updateCount = 0;
		UpdateMenu();
	}
}

// On open register the menu and the events
void DurabilityMenu::OnMenuOpen() {
	auto utility = Utility::GetSingleton();
	if (!ms_pSingleton) {
		if (uiMovie) {
			RE::GFxValue args[4];
			args[0].SetNumber(ini.GetWidgetSettings("PositionX") / 100.0);
			args[1].SetNumber(ini.GetWidgetSettings("PositionY") / 100.0);
			args[2].SetNumber((double)ini.GetWidgetSettings("Scale"));
			args[3].SetNumber((double)ini.GetWidgetSettings("Flags"));

			uiMovie->Invoke("_root.widget.Setup", nullptr, args, 4);
			logger::debug("Set root of loaded movie");
		}

		RE::UI *ui = RE::UI::GetSingleton();
		ui->GetEventSource<RE::MenuOpenCloseEvent>()->AddEventSink(this);

		skseActivated = false;
		hotkeyActivated = false;

		if (ini.GetWidgetSettings("HideWithWeaponsSheathed") == 1) {
			auto messaging = SKSE::GetMessagingInterface();
			auto actionDispatch = (RE::BSTEventSource<SKSE::ActionEvent>*)messaging->GetEventDispatcher(SKSE::MessagingInterface::Dispatcher::kActionEvent);
			actionDispatch->AddEventSink(this);

			if (utility->GetPlayer()->AsActorState()->IsWeaponDrawn()) {
				visible = true;
				skseActivated = true;
			} else {
				visible = false;
				skseActivated = false;
			}
		}

		ms_pSingleton = this;

		if (ini.GetWidgetSettings("ToggleKeyCode") != 0) {
			InputEventHandler::Register();

			if (ini.GetWidgetSettings("ToggleDuration") > 0)
				visible = false;
			else
				visible = true;
		}
	}
}

// On close, kill the events and the singleton pointer
void DurabilityMenu::OnMenuClose() {
	if (ms_pSingleton) {
		ms_pSingleton = nullptr;

		RE::UI *ui = RE::UI::GetSingleton();
		ui->RemoveEventSink<RE::MenuOpenCloseEvent>(this);

		if (ini.GetWidgetSettings("HideWithWeaponsSheathed") == 1) {
			auto messaging = SKSE::GetMessagingInterface();
			auto actionDispatch = (RE::BSTEventSource<SKSE::ActionEvent>*)messaging->GetEventDispatcher(SKSE::MessagingInterface::Dispatcher::kActionEvent);
			actionDispatch->RemoveEventSink(this);
		}

		if (ini.GetWidgetSettings("ToggleKeyCode") != 0)
			InputEventHandler::Unregister();
	}

	AutoOpenDurabilityMenuSink::Register();
}

// Update the SWF based on what the user currently has equipped
void DurabilityMenu::UpdateMenu() {
	auto utility = Utility::GetSingleton();
	if (!uiMovie || !uiMovie->GetVisible())
		return;

	std::vector<FoundEquipData> eqDs;
	eqDs.reserve(7);

	// Look for player inventory changes or visibile. Equipment will default to blank on !visible
	RE::InventoryChanges *exChanges = utility->GetPlayer()->GetInventoryChanges();
	if (!exChanges || !visible) {
		FoundEquipData eqD;
		for (int i = 0; i < 7; i++) {
			eqDs.push_back(eqD);
		}
	} else {
		FoundEquipData eqD_head = FoundEquipData::FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kHead);
		if (eqD_head.pForm)
			eqDs.push_back(eqD_head);
		else
			eqDs.push_back(FoundEquipData::FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kHair));

		eqDs.push_back(FoundEquipData::FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kBody));
		eqDs.push_back(FoundEquipData::FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kHands));
		eqDs.push_back(FoundEquipData::FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kFeet));

		RE::TESForm* left = utility->GetPlayer()->GetEquippedObject(true);
		if (left) {
			if (left->IsWeapon()) {
				eqDs.push_back(FoundEquipData::FindEquippedWeapon(exChanges, true, left));
			} else if (left->IsArmor() && left->As<RE::TESObjectARMO>()->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kShield)) {
				eqDs.push_back(FoundEquipData::FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kShield));
			} else {
				eqDs.push_back(FoundEquipData(left));
			}
		} else {
			eqDs.push_back(FoundEquipData());
		}

		RE::TESForm* right = utility->GetPlayer()->GetEquippedObject(false);
		if (right) {
			if (right->IsWeapon()) {
				eqDs.push_back(FoundEquipData::FindEquippedWeapon(exChanges, false, right));
			} else {
				eqDs.push_back(FoundEquipData(right));
			}
		} else {
			eqDs.push_back(FoundEquipData());
		}

		eqDs.push_back(FoundEquipData(utility->GetPlayer()->GetActorRuntimeData().selectedPower));
	}

	int i = 0;
	RE::GFxValue args[28];
	for (FoundEquipData &eqD : eqDs)
	{
		// Place holders before inserting into the menu arguments
		const char* icon;
		const char* text;
		double value;
		double color;

		// Default values if there are no forms
		if (!eqD.pForm) {
			icon = strIcons[kIcon_None];
			text = "";
			value = -1;
			color = (double)ini.GetWidgetSettings("UnbreakableColor");
		} else {

			if (eqD.pForm->IsWeapon()) {
				RE::TESObjectWEAP *weap = eqD.pForm->As<RE::TESObjectWEAP>();
				if (weap->IsOneHandedSword())
					icon = strIcons[kIcon_WeaponSword];
				else if (weap->IsOneHandedDagger())
					icon = strIcons[kIcon_WeaponDagger];
				else if (weap->IsOneHandedAxe())
					icon = strIcons[kIcon_WeaponWarAxe];
				else if (weap->IsOneHandedMace())
					icon = strIcons[kIcon_WeaponMace];
				else if (weap->IsTwoHandedSword())
					icon = strIcons[kIcon_WeaponGreatSword];
				else if (weap->IsTwoHandedAxe()) {
					if (weap->HasKeyword(utility->keywordWarhammer))
						icon = strIcons[kIcon_WeaponHammer];
					else
						icon = strIcons[kIcon_WeaponBattleAxe];
				} else if (weap->IsBow())
					icon = strIcons[kIcon_WeaponBow];
				else if (weap->IsStaff())
					icon = strIcons[kIcon_WeaponStaff];
				else if (weap->IsCrossbow())
					icon = strIcons[kIcon_WeaponCrossbow];
				else
					icon = strIcons[kIcon_WeaponSword];

				eqD.GenerateName();
				text = eqD.name.c_str();

				if (eqD.CanTemper()) {
					float itemHealthPercent = eqD.GetItemHealthPercent();
					value = std::max(std::round((itemHealthPercent - 1.0f) * 1000.0), 0.0);
					if (ini.GetBreakChanceSettings(eqD.pForm) != 0.0 && itemHealthPercent <= ini.EquipmentHealthThreshold && eqD.CanBreak())
						color = (double)ini.GetWidgetSettings("BreakableColor");
					else
						color = (double)ini.GetWidgetSettings("UnbreakableColor");
				} else {
					value = -1;
					color = (double)ini.GetWidgetSettings("UnbreakableColor");
				}

			} else if (eqD.pForm->IsArmor()) {
				RE::TESObjectARMO *armor = eqD.pForm->As<RE::TESObjectARMO>();
				
				static Icon icons[] = {
					kIcon_LightArmorBody,		// 0
					kIcon_LightArmorHead,
					kIcon_LightArmorHands,
					kIcon_LightArmorForearms,
					kIcon_LightArmorFeet,
					kIcon_LightArmorCalves,
					kIcon_LightArmorShield,
					kIcon_LightArmorMask,

					kIcon_ArmorBody,			// 8
					kIcon_ArmorHead,
					kIcon_ArmorHands,
					kIcon_ArmorForearms,
					kIcon_ArmorFeet,
					kIcon_ArmorCalves,
					kIcon_ArmorShield,
					kIcon_ArmorMask,

					kIcon_ClothingBody,		// 16
					kIcon_ClothingHead,
					kIcon_ClothingHands,
					kIcon_ClothingForearms,
					kIcon_ClothingFeet,
					kIcon_ClothingCalves,
					kIcon_ClothingShield,
					kIcon_ClothingMask,

					kIcon_ArmorAmulet,		// 24
					kIcon_ArmorRing,
					kIcon_Circlet,

					kIcon_DefaultArmor		// 27
				};

				int index = 0;

				if (armor->IsLightArmor()) {
					index = 0;
				} else if (armor->IsHeavyArmor()) {
					index = 8;
				} else {
					if (armor->HasKeyword(utility->keywordClothing))
					{
						index = 16;
					} else if (armor->HasKeyword(utility->keywordJewelry)) {
						if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kAmulet))
							index = 24;
						else if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kRing))
							index = 25;
						else if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kCirclet))
							index = 26;
						else
							index = 27;
					} else {
						index = 27;
					}
				}

				if (index < 24)
				{
					if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kBody) || armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kTail))
						index += 0;
					else if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHead) || armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHair) || armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kLongHair)) {
						index += 1;
						if (armor->formID >= 0x061C8B && armor->formID < 0x061CD7)
							index += 6;
					} else if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHands))
						index += 2;
					else if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kForearms))
						index += 3;
					else if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kFeet))
						index += 4;
					else if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kCalves))
						index += 5;
					else if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kShield))
						index += 6;
					else
						index = 27;
				}

				icon = strIcons[icons[index]];

				eqD.GenerateName();
				text = eqD.name.c_str();

				float itemHealthPercent = eqD.GetItemHealthPercent();
				value = std::max(std::round((itemHealthPercent - 1.0f) * 1000.0), 0.0);
				if (ini.GetBreakChanceSettings(eqD.pForm) != 0.0 && itemHealthPercent <= ini.EquipmentHealthThreshold && eqD.CanBreak())
					color = (double)ini.GetWidgetSettings("BreakableColor");
				else
					color = (double)ini.GetWidgetSettings("UnbreakableColor");
			
			} else if (eqD.pForm->formType == RE::FormType::Spell) {

				RE::SpellItem *spell = eqD.pForm->As<RE::SpellItem>();
				if (spell->GetSpellType() == RE::MagicSystem::SpellType::kPower || spell->GetSpellType() == RE::MagicSystem::SpellType::kLesserPower) {
					icon = strIcons[kIcon_Default_Power];
				} else if (spell->GetAssociatedSkill() == RE::ActorValue::kAlteration)
					icon = strIcons[kIcon_Default_Alteration];
				else if (spell->GetAssociatedSkill() == RE::ActorValue::kIllusion)
					icon = strIcons[kIcon_Default_Illusion];
				else if (spell->GetAssociatedSkill() == RE::ActorValue::kDestruction)
					icon = strIcons[kIcon_Default_Destruction];
				else if (spell->GetAssociatedSkill() == RE::ActorValue::kConjuration)
					icon = strIcons[kIcon_Default_Conjuration];
				else if (spell->GetAssociatedSkill() == RE::ActorValue::kRestoration)
					icon = strIcons[kIcon_Default_Restoration];
				else
					icon = strIcons[kIcon_Default_Effect];

				text = eqD.pForm->GetName();
				value = -1;
				color = (double)ini.GetWidgetSettings("UnbreakableColor");

			} else if (eqD.pForm->formType == RE::FormType::Shout) {
				icon = strIcons[kIcon_Default_Shout];
				text = eqD.pForm->GetName();
				value = -1;
				color = (double)ini.GetWidgetSettings("UnbreakableColor");
			} else if (eqD.pForm->formType == RE::FormType::Scroll) {
				icon = strIcons[kIcon_DefaultScroll];
				text = eqD.pForm->GetName();
				value = -1;
				color = (double)ini.GetWidgetSettings("UnbreakableColor");
			} else if (eqD.pForm->formType == RE::FormType::Light) {
				icon = strIcons[kIcon_MiscTorch];
				text = eqD.pForm->GetName();
				value = -1;
				color = (double)ini.GetWidgetSettings("UnbreakableColor");
			} else {
				icon = strIcons[kIcon_DefaultMisc];
				text = eqD.pForm->GetName();
				value = -1;
				color = (double)ini.GetWidgetSettings("UnbreakableColor");
			}
		}

		args[i].SetString(icon);
		args[i+1].SetString(text);
		args[i+2].SetNumber(value);
		args[i+3].SetNumber(color);
		
		i += 4;
	}

	uiMovie->Invoke("_root.widget.UpdateMenu", nullptr, args, 28);
}