#pragma once

#include "RE/B/BGSSaveLoadGame.h"

namespace RE
{
	class EXBGSSaveLoadGame : public BGSSaveLoadGame
	{
	public:
		static EXBGSSaveLoadGame* GetSingleton();

		void ClearForm(TESForm* a_form);

		// members
		std::uint64_t unk000[105];
	};
	static_assert(sizeof(BGSSaveLoadGame) == 0x348);
}
