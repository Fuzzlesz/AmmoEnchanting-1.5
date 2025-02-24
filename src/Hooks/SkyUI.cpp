#include "SkyUI.h"

#include "RE/Offset.h"

#include <xbyak/xbyak.h>

namespace Hooks
{
	void SkyUI::Install()
	{
		FilterFlagPatch();
		ItemDataPatch();
	}

	void SkyUI::FilterFlagPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingMenu::ProcessMessage,
			0x3E7);

		if (!REL::make_pattern<"E8">().match(hook.address())) {
			logger::error("SkyUI::FilterFlagPatch failed to install"sv);
			return;
		}

		auto& trampoline = SKSE::GetTrampoline();
		_EnchantConstructMenu_ctor = trampoline.write_call<5>(
			hook.address(),
			&SkyUI::SetupSubMenu);
	}

	void SkyUI::ItemDataPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::PopulateEntryList,
			0xDF);

		if (!REL::make_pattern<"4C 8D 44 24 38 48 8B 56 78">().match(hook.address())) {
			logger::error("SkyUI::ItemDataPatch failed to install"sv);
			return;
		}

		struct Patch : Xbyak::CodeGenerator
		{
			Patch()
			{
				// mov(r8, rsi);
				lea(rdx, ptr[rbp - 0x29]);
				mov(rcx, ptr[rbx]);
				mov(rax, util::function_ptr(&SkyUI::SetItemData));
				call(rax);

				lea(r8, ptr[rbp - 0x29]);
				mov(rdx, ptr[rsi + offsetof(Menu, entryList) + 0x10]);  // entryList._value

				jmp(ptr[rip]);
				dq(hook.address() + 0x9);
			}
		};

		Patch patch{};
		patch.ready();

		REL::safe_fill(hook.address(), REL::NOP, 0x9);

		auto& trampoline = SKSE::GetTrampoline();
		trampoline.write_branch<6>(hook.address(), trampoline.allocate(patch));
	}

	RE::CraftingSubMenus::EnchantConstructMenu* SkyUI::SetupSubMenu(
		RE::CraftingSubMenus::EnchantConstructMenu* a_subMenu,
		RE::GPtr<RE::GFxMovieView>& a_view,
		RE::TESFurniture* a_furniture)
	{
		RE::GFxValue config;
		a_view->GetVariable(&config, "skyui.util.ConfigManager._config");
		if (config.IsObject()) {
			OverrideConfig(config);
		}

		return _EnchantConstructMenu_ctor(a_subMenu, a_view, a_furniture);
	}

	void SkyUI::OverrideConfig(RE::GFxValue& a_config)
	{
		RE::GFxValue listLayout;
		a_config.GetMember("ListLayout", &listLayout);
		if (!listLayout.IsObject()) {
			return;
		}

		RE::GFxValue views;
		listLayout.GetMember("views", &views);
		if (!views.IsObject()) {
			return;
		}

		RE::GFxValue view;
		RE::GFxValue category;

		views.GetMember("disenchantView", &view);
		if (view.IsObject()) {
			category.SetNumber(FilterFlag::Disenchant);
			view.SetMember("category", category);
		}

		views.GetMember("enchantItemView", &view);
		if (view.IsObject()) {
			category.SetNumber(FilterFlag::Item);
			view.SetMember("category", category);
		}

		views.GetMember("enchantEnchantmentView", &view);
		if (view.IsObject()) {
			category.SetNumber(FilterFlag::Enchantment);
			view.SetMember("category", category);
		}
	}

	void SkyUI::SetItemData(Menu::CategoryListEntry* a_entry, RE::GFxValue* a_dataContainer)
	{
		switch (a_entry->filterFlag.underlying()) {

		case FilterFlag::EnchantAmmo:
		case FilterFlag::DisenchantAmmo:
		{
			const auto entry = static_cast<Menu::ItemChangeEntry*>(a_entry);
			const auto& data = entry->data;

			if (!data || !data->object)
				break;

			const auto& form = data->object;
			const auto player = RE::PlayerCharacter::GetSingleton();

			a_dataContainer->SetMember("formType", form->GetFormType());
			a_dataContainer->SetMember("formId", form->formID);

			a_dataContainer->SetMember("value", form->GetGoldValue());
			a_dataContainer->SetMember("weight", form->GetWeight());
			a_dataContainer->SetMember("isStolen", !data->IsOwnedBy(player));

			if (const auto ammo = form->As<RE::TESAmmo>()) {
				a_dataContainer->SetMember("flags", ammo->data.flags.underlying());
				a_dataContainer->SetMember("damage", player->GetDamage(data));

				const auto& projectile = ammo->data.projectile;
				const auto explosion = projectile ? projectile->data.explosionType : nullptr;
				const auto enchantment = explosion ? explosion->formEnchanting : nullptr;

				if (enchantment) {
					RE::GFxValue name;
					name.SetString(enchantment->GetName());
					a_dataContainer->SetMember("effectName", name);
				}
			}
		} break;

		case FilterFlag::EffectAmmo:
		{
			const auto entry = static_cast<Menu::EnchantmentEntry*>(a_entry);
			const auto& form = entry->data;

			if (!form)
				break;

			a_dataContainer->SetMember("formType", form->GetFormType());
			a_dataContainer->SetMember("formId", form->formID);

			RE::GFxValue spellName;
			spellName.SetString(form->GetName());
			a_dataContainer->SetMember("spellName", spellName);

			const auto effect = form->GetCostliestEffectItem();
			if (effect && effect->baseEffect) {
				a_dataContainer->SetMember("magnitude", effect->effectItem.magnitude);
				a_dataContainer->SetMember("duration", effect->effectItem.duration);
				a_dataContainer->SetMember("area", effect->effectItem.area);

				const auto& baseEffect = effect->baseEffect;

				RE::GFxValue effectName;
				effectName.SetString(baseEffect->GetName());
				a_dataContainer->SetMember("effectName", effectName);

				a_dataContainer->SetMember("subType", baseEffect->GetMagickSkill());
				a_dataContainer->SetMember("effectFlags", baseEffect->data.flags.underlying());
				a_dataContainer->SetMember("school", baseEffect->GetMagickSkill());
				a_dataContainer->SetMember("skillLevel", baseEffect->GetMinimumSkillLevel());
				a_dataContainer->SetMember("archetype", baseEffect->GetArchetype());
				a_dataContainer->SetMember("deliveryType", baseEffect->data.delivery);
				a_dataContainer->SetMember("castTime", baseEffect->data.spellmakingChargeTime);
				a_dataContainer->SetMember("delayTime", baseEffect->data.aiDelayTimer);
				a_dataContainer->SetMember("actorValue", baseEffect->data.primaryAV);
				a_dataContainer->SetMember("castType", baseEffect->data.castingType);
				a_dataContainer->SetMember("magicType", baseEffect->data.resistVariable);
			}
		} break;
		}
	}
}
