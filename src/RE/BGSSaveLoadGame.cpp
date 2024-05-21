#include "RE/BGSSaveLoadGame.h"
#include "RE/B/BGSSaveLoadGame.h"

#include "RE/Offset.h"

namespace RE
{
	EXBGSSaveLoadGame* EXBGSSaveLoadGame::GetSingleton()
	{
		REL::Relocation<BGSSaveLoadGame**> singleton{ Offset::EXBGSSaveLoadGame::Singleton };
		return static_cast<EXBGSSaveLoadGame*>(*singleton);
	}

	void EXBGSSaveLoadGame::ClearForm(TESForm* a_form)
	{
		using func_t = decltype(&EXBGSSaveLoadGame::ClearForm);
		REL::Relocation<func_t> func{ Offset::EXBGSSaveLoadGame::ClearForm };
		return func(this, a_form);
	}
}
