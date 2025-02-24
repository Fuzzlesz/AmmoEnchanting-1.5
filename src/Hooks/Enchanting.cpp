#include "Enchanting.h"

#include "Data/CreatedObjectManager.h"
#include "Data/EnchantArtManager.h"
#include "Ext/EnchantConstructMenu.h"
#include "RE/Offset.h"
#include "Settings/INISettings.h"
#include "Translation/Translation.h"

#include <xbyak/xbyak.h>

namespace Hooks
{
	void Enchanting::Install()
	{
		ItemPreviewPatch();
		CraftItemPatch();
		EnchantArtPatch();
		EnchantConfirmPatch();
		AmmoQuantityPatch();
		InventoryNotificationPatch();
		EnchantmentChargePatch();
	}

	void Enchanting::ItemPreviewPatch()
	{
		static const auto hook1 = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::Update,
			0x2A5);

		static const auto hook2 = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::Update,
			0x2C0);

		if (!REL::make_pattern<"41 83 FF 29">().match(hook1.address()) ||
			!REL::make_pattern<"41 83 FF 1A">().match(hook2.address())) {

			util::report_and_fail("Enchanting::ItemPreviewPatch failed to install"sv);
		}

		// jmp from 0x2A5 to 0x2C0
		std::array<std::uint8_t, 5> patch1{ 0xE9, 0x16, 0x00, 0x00, 0x00 };

		struct Patch : Xbyak::CodeGenerator
		{
			Patch()
			{
				Xbyak::Label retn;

				mov(rdx, rsi);
				mov(ecx, r15d);
				mov(rax, util::function_ptr(&Enchanting::CreateEnchantment));
				call(rax);
				test(rax, rax);
				jz(retn);
				nop(0x2);
				// hook + 0x19
				mov(ptr[rsi + offsetof(Menu, createEffectFunctor) +
						offsetof(Menu::CreateEffectFunctor, createdEnchantment)],
					rax);
				// hook + 0x20
				L(retn);
			}
		};

		Patch patch2{};
		patch2.ready();

		assert(patch2.getSize() == 0x20);

		REL::safe_fill(hook1.address(), REL::NOP, 0x22);
		REL::safe_write(hook1.address(), patch1.data(), patch1.size());
		REL::safe_write(hook2.address(), patch2.getCode(), patch2.getSize());
	}

	void Enchanting::CraftItemPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::CraftItem,
			0x9D);

		if (!REL::make_pattern<"8B 43">().match(hook.address())) {
			util::report_and_fail("Enchanting::CraftItemPatch failed to install"sv);
		}

		struct Patch : Xbyak::CodeGenerator
		{
			Patch()
			{
				mov(rdx, rsi);
				mov(ecx,
					dword
						[rsi + offsetof(Menu, createEffectFunctor) +
						 offsetof(Menu::CreateEffectFunctor, formType)]);
				mov(rax, util::function_ptr(&Enchanting::CreateEnchantment));
				call(rax);
			}
		};

		Patch patch{};

		assert(patch.getSize() <= 0x35);

		REL::safe_fill(hook.address(), REL::NOP, 0x35);
		REL::safe_write(hook.address(), patch.getCode(), patch.getSize());
	}

	void Enchanting::EnchantArtPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::CraftItem,
			0x17E);

		if (!REL::make_pattern<"E8">().match(hook.address())) {
			util::report_and_fail("Enchanting::EnchantArtPatch failed to install"sv);
		}

		auto& trampoline = SKSE::GetTrampoline();
		_UpdateWeaponAbility = trampoline.write_call<5>(hook.address(), &UpdateWeaponAbility);
	}

	void Enchanting::EnchantConfirmPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::ProcessUserEvent,
			0x194);

		if (!REL::make_pattern<"E8">().match(hook.address())) {
			logger::error("Enchanting::EnchantConfirmPatch failed to install"sv);
			return;
		}

		auto& trampoline = SKSE::GetTrampoline();
		_SetStr = trampoline.write_call<5>(hook.address(), &Enchanting::SetConfirmText);
	}

	void Enchanting::AmmoQuantityPatch()
	{
		static const auto hook1 = REL::Relocation<std::uintptr_t>(
			RE::Offset::InventoryChanges::EnchantObject,
			0x8C);

		static const auto hook2 = REL::Relocation<std::uintptr_t>(
			RE::Offset::InventoryChanges::EnchantObject,
			0xFB);

		static const auto hook3 = REL::Relocation<std::uintptr_t>(
			RE::Offset::InventoryChanges::EnchantObject,
			0x15B);

		if (!REL::make_pattern<"E8">().match(hook1.address()) ||
			!REL::make_pattern<"E8">().match(hook2.address()) ||
			!REL::make_pattern<"E8">().match(hook3.address())) {

			util::report_and_fail("Enchanting::AmmoQuantityPatch failed to install"sv);
		}

		auto& trampoline = SKSE::GetTrampoline();

		_GetCount = trampoline.write_call<5>(hook1.address(), &Enchanting::GetCount);

		_CopyExtraData = trampoline.write_call<5>(hook2.address(), &Enchanting::CopyExtraData);

		_SetEnchantment = trampoline.write_call<5>(hook3.address(), &Enchanting::SetExtraData);
	}

	void Enchanting::InventoryNotificationPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::CraftItem,
			0x2A5);

		if (!REL::make_pattern<"E8">().match(hook.address())) {
			logger::error("Enchanting::InventoryNotificationPatch failed to install"sv);
			return;
		}

		auto& trampoline = SKSE::GetTrampoline();

		_InventoryNotification = trampoline.write_call<5>(
			hook.address(),
			&Enchanting::InventoryNotification);
	}

	void Enchanting::EnchantmentChargePatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::Update,
			0x202);

		if (!REL::make_pattern<"E8">().match(hook.address())) {
			util::report_and_fail("Enchanting::EnchantmentChargePatch failed to install"sv);
		}

		auto& trampoline = SKSE::GetTrampoline();

		_ApplyPerkEntries = trampoline.write_call<5>(
			hook.address(),
			&Enchanting::ApplyPerkEntries);
	}

	RE::EnchantmentItem* Enchanting::CreateEnchantment(
		RE::FormType a_formType,
		RE::CraftingSubMenus::EnchantConstructMenu* a_menu)
	{
		_customName = a_menu->customName.c_str();
		_availableCount = a_menu->selected.item
			? static_cast<std::uint16_t>(a_menu->selected.item->data->countDelta)
			: 0;

		switch (a_formType) {

		case RE::FormType::Armor:
			_creatingCount = 1;
			return RE::BGSCreatedObjectManager::GetSingleton()->AddArmorEnchantment(
				a_menu->createEffectFunctor.createdEffects);

		case RE::FormType::Weapon:
			_creatingCount = 1;
			return RE::BGSCreatedObjectManager::GetSingleton()->AddWeaponEnchantment(
				a_menu->createEffectFunctor.createdEffects);

		case RE::FormType::Ammo:
			_creatingCount = Ext::EnchantConstructMenu::GetAmmoEnchantQuantity(a_menu);
			return Data::CreatedObjectManager::GetSingleton()->CreateAmmoEnchantment(
				a_menu->createEffectFunctor.createdEffects);

		default:
			return nullptr;
		}
	}

	void Enchanting::UpdateWeaponAbility(
		RE::Actor* a_actor,
		RE::TESForm* a_item,
		RE::ExtraDataList* a_extraList,
		bool a_wornLeft)
	{
		if (a_item->IsWeapon()) {
			_UpdateWeaponAbility(a_actor, a_item, a_extraList, a_wornLeft);
		}
		else if (a_item->IsAmmo()) {
			const auto exEnchantment = a_extraList->GetByType<RE::ExtraEnchantment>();
			const auto enchantment = exEnchantment ? exEnchantment->enchantment : nullptr;

			Data::EnchantArtManager::GetSingleton()->UpdateAmmoEnchantment(a_actor, enchantment);
		}
	}

	void Enchanting::SetConfirmText(RE::BSString* a_str, const char* a_sEnchantItem)
	{
		if (_availableCount < _creatingCount) {
			std::string fmt;
			if (!Translation::ScaleformTranslate("$AMEN_NotEnoughArrows{}{}"s, fmt)) {
				fmt = "Enchantment charges: {}. You only have {} of the selected item."s;
			}

			std::string msg = fmt::vformat(
				fmt,
				fmt::make_format_args(_creatingCount, _availableCount));

			_SetStr(a_str, msg.data());
		}
		else {
			_SetStr(a_str, a_sEnchantItem);
		}
	}

	std::uint16_t Enchanting::GetCount(RE::ExtraDataList* a_extraList)
	{
		std::uint16_t count = _GetCount(a_extraList);
		if (_creatingCount > count) {
			_creatingCount = count;
		}

		return count - _creatingCount + 1;
	}

	void Enchanting::CopyExtraData(RE::ExtraDataList* a_target, RE::ExtraDataList* a_source)
	{
		if (_creatingCount > 1) {
			auto count = static_cast<std::uint16_t>(a_source->GetCount());
			a_source->SetCount(count + 1 - _creatingCount);
		}

		_CopyExtraData(a_target, a_source);

		// Weapons and armor can't have both ExtraCount and ExtraWorn, so it's assumed for them
		// that if this code is running, ExtraWorn isn't in the list. This isn't the case for
		// ammo, so we need to remove ExtraWorn from the new list being created.
		a_target->RemoveByType(RE::ExtraDataType::kWorn);
	}

	void Enchanting::SetExtraData(
		RE::ExtraDataList* a_extraList,
		RE::EnchantmentItem* a_enchantment,
		std::uint16_t a_chargeAmount,
		bool a_removeOnUnequip)
	{
		_SetEnchantment(a_extraList, a_enchantment, a_chargeAmount, a_removeOnUnequip);

		if (_creatingCount > _availableCount) {
			_creatingCount = _availableCount;
		}

		if (_creatingCount > 1) {
			a_extraList->SetCount(_creatingCount);
		}
	}

	void Enchanting::InventoryNotification(
		RE::TESForm* a_item,
		[[maybe_unused]] std::int32_t a_count,
		bool a_itemAdded,
		bool a_playSound,
		const char* a_name)
	{
		// Vanilla only shows the original item name, so patch that here
		const char* name = a_name;
		if (_customName && _customName[0] != '\0') {
			name = _customName;
		}

		_InventoryNotification(a_item, _creatingCount, a_itemAdded, a_playSound, name);
	}

	void Enchanting::ApplyPerkEntries(
		RE::BGSEntryPoint::ENTRY_POINT a_entryPoint,
		RE::Actor* a_perkOwner,
		RE::EnchantmentItem* a_enchantment,
		RE::TESForm* a_item,
		float& a_value)
	{
		assert(a_entryPoint == RE::BGSEntryPoint::ENTRY_POINT::kModSoulGemEnchanting);

		_ApplyPerkEntries(a_entryPoint, a_perkOwner, a_enchantment, a_item, a_value);

		if (Data::CreatedObjectManager::GetSingleton()->IsBaseAmmoEnchantment(a_enchantment)) {
			a_value *= Settings::INISettings::GetSingleton()->fAmmoChargeMult;
		}
	}
}
