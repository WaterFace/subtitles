#include "SubtitleManager.h"

namespace Subtitles
{
	class UpdatePCHook
	{
	public:
		static void Install()
		{
			REL::Relocation<std::uintptr_t> pcVTable{ RE::VTABLE_PlayerCharacter[0] };
			UpdatePC = pcVTable.write_vfunc(0xAD, UpdatePCMod);
			logger::info("PlayerCharacter::Update hooked");
		}

	private:
		static void UpdatePCMod(RE::PlayerCharacter* pc, float delta)
		{
			// call original function
			UpdatePC(pc, delta);

			auto myManager = SubtitleManager::GetSingleton();
			auto subtitleManager = RE::SubtitleManager::GetSingleton();
			for (RE::SubtitleInfo& subtitleInfo : subtitleManager->subtitles) {
				myManager->AddSubtitle(&subtitleInfo);
			}

			myManager->ShowSubtitles();
		}
		static inline REL::Relocation<decltype(UpdatePCMod)> UpdatePC;
	};

	// Hook into the Invoke calls in HUDMenu::ProcessMessage which call "HideSubtitle" and "ShowSubtitle"
	class InvokeHook
	{
	public:
		static void Install()
		{
			auto address = REL::VariantID(50718, 51612, 0).address();

			auto offset = REL::VariantOffset(0x756, 0x77E, 0).offset();
			SKSE::GetTrampoline().write_call<5>(address + offset, InvokeModHide);

			offset = REL::VariantOffset(0x703, 0x72B, 0).offset();
			SKSE::GetTrampoline().write_call<5>(address + offset, InvokeModShow);
			logger::info("Invoke calls in HUDMenu::ProcessMessage hooked");
		}

	private:
		static bool InvokeModHide(RE::GFxValue::ObjectInterface* objInt, void* a_data, RE::GFxValue* a_result, const char* a_name, const RE::GFxValue* a_args, RE::UPInt a_numArgs, bool isDObj)
		{
			// We have to check this since multiple message types pass through this branch
			if (strcmp(a_name, "HideSubtitle") == 0) {
				return true;
			}

			return objInt->Invoke(a_data, a_result, a_name, a_args, a_numArgs, isDObj);
		}

		static bool InvokeModShow(RE::GFxValue::ObjectInterface* objInt, void* a_data, RE::GFxValue* a_result, const char* a_name, const RE::GFxValue* a_args, RE::UPInt a_numArgs, bool isDObj)
		{
			// We don't have to check here since this one only handles "ShowSubtitle" invocations
			return objInt->Invoke(a_data, a_result, a_name, a_args, a_numArgs, isDObj);
		}
	};

	namespace Hooks
	{
		void Install()
		{
			Subtitles::UpdatePCHook::Install();
			if (!REL::Module::IsVR()) {
				Subtitles::InvokeHook::Install();
			} else {
				logger::info("Skipping Invoke hook because I don't have the VR offset yet!");
			}
		}
	}
}