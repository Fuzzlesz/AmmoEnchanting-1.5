#pragma once

namespace RE::Offset
{
	namespace Actor
	{
		inline constexpr REL::ID AddWornItem{ 36979 };					// 14060CA30
		inline constexpr REL::ID AutoEquipNewAmmo{ 40241 };				// 1406CFC00
	}

	namespace ArrowProjectile
	{
		inline constexpr REL::ID Vtbl{ 263776 };						// 141676318
	}

	namespace BGSCreatedObjectManager
	{
		inline constexpr REL::ID CreateEnchantment{ 35282 };			// 1405A0EB0
	}

	namespace BSResource
	{
		inline constexpr REL::ID FreeRequestedModel{ 11275 };           // 1401036A0
		inline constexpr REL::ID RequestModelAsync{ 74038 };			// 140D2EE20
		inline constexpr REL::ID RequestModelDirect{ 74039 };           // 140D2EFE0
	}

	namespace EXBGSSaveLoadGame
	{
		inline constexpr REL::ID ClearForm{ 34665 };					// 14057C800
		inline constexpr REL::ID Singleton{ 516851 };					// 142F266F8
	}

	namespace Character
	{
		inline constexpr REL::ID Vtbl{ 261397 };						// 14165DA40
	}

	namespace CraftingMenu
	{
		inline constexpr REL::ID ProcessMessage{ 50274 };				// 140862C80
	}

	namespace CraftingSubMenus
	{
		namespace EnchantConstructMenu
		{
			inline constexpr REL::ID Ctor{ 50329 };						// 140866E20
			inline constexpr REL::ID AddEnchantmentIfKnown{ 50368 };    // 140868C20
			inline constexpr REL::ID CalculateCharge{ 50434 };			// 14086AC60
			inline constexpr REL::ID CanSelectEntry{ 50569 };           // 140874A40
			inline constexpr REL::ID CraftItem{ 50450 };                // 14086C640
			inline constexpr REL::ID DisenchantItem{ 50459 };           // 14086D830
			inline constexpr REL::ID Filter{ 50454 };			        // 14086D0A0
			inline constexpr REL::ID PopulateEntryList{ 50311 };        // 140864C50
			inline constexpr REL::ID ProcessUserEvent{ 50487 };         // 14086EE80
			inline constexpr REL::ID SelectEntry{ 50440 };              // 14086B200
			inline constexpr REL::ID SliderClose{ 50550 };              // 140872E00
			inline constexpr REL::ID Update{ 50557 };                   // 140873460
			inline constexpr REL::ID UpdateEnabledEntries{ 50558 };     // 140873910

			namespace CreateEffectFunctor
			{
				inline constexpr REL::ID Invoke{ 50366 };				// 140868A00
			}

			namespace Selections
			{
				inline constexpr REL::ID SelectEntry{ 50524 };			// 140870FD0
			}
		}
	}

	namespace EnchantmentItem
	{
		inline constexpr REL::ID Create{ 35276 };						// 1405A0760
	}

	namespace ExtraDataList
	{
		inline constexpr REL::ID LoadGame{ 11981 };						// 14012DCD0
	}

	namespace InventoryChanges
	{
		inline constexpr REL::ID EnchantObject{ 15906 };				// 1401EC990
	}

	namespace InventoryEntryData
	{
		inline constexpr REL::ID GetEnchantment{ 15757 };				// 1401D66E0
	}

	namespace ItemCard
	{
		inline constexpr REL::ID GetMagicItemDescription{ 51022 };		// 140892CD0
	}

	namespace MissileProjectile
	{
		inline constexpr REL::ID UpdateImpl{ 42852 };					// 140744DD0
	}

	namespace NiAVObject
	{
		inline constexpr REL::ID Clone{ 68835 };						// 140C52750
		inline constexpr REL::ID SetValueNodeHidden{ 19215 };			// 14028C2B0
	}

	namespace PlayerCharacter
	{
		inline constexpr REL::ID SwitchSkeleton{ 39401 };				// 1406A1820
		inline constexpr REL::ID UseAmmo{ 40240 };						// 1406CFB80
	}

	namespace Projectile
	{
		inline constexpr REL::ID GetSpeed{ 42958 };						// 14074DC20
		inline constexpr REL::ID Launch{ 42928 };						// 14074B170
		inline constexpr REL::ID Initialize{ 42940 };                   // 14074C310
	}

	namespace ScriptFunctions
	{
		inline constexpr REL::ID PlayerEnchantObject{ 22024 };			// 1403055C0
	}

	namespace StandardItemData
	{
		inline constexpr REL::ID Init{ 50071 };							// 140856050
	}

	namespace TESForm
	{
		inline constexpr REL::ID GetEnchantment{ 14411 };				// 140190D50
	}

	inline constexpr REL::ID ShowHUDAmmo{ 50734 };						// 14087F600
	inline constexpr REL::ID UnarmedWeapon{ 514923 };					// 142EFF868
}
