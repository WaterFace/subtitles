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

	// Hook into calls to UIMessageQueue::AddMessage with message data corresponding to a "HideSubtitles" invocation
	class HideSubtitlesHook
	{
	public:
		static void Install()
		{
			// VR PRs welcome!
			auto address = REL::VariantID(51755, 52629, 0).address();
			auto offset = REL::VariantOffset(0x9A, 0xA0, 0).offset();
			AddMessage = SKSE::GetTrampoline().write_call<5>(address + offset, AddMessageMod);
			logger::info("UIMessageQueue::AddMessage hooked");
		}

	private:
		static void AddMessageMod(
			[[maybe_unused]] RE::UIMessageQueue* queue,
			[[maybe_unused]] RE::BSFixedString* menu,
			[[maybe_unused]] RE::UI_MESSAGE_TYPE type,
			[[maybe_unused]] RE::HUDData* data)
		{
			// do nothing on purpose
		}
		static inline REL::Relocation<decltype(AddMessageMod)> AddMessage;
	};

	namespace Hooks
	{
		void Install()
		{
			Subtitles::UpdatePCHook::Install();
			if (!REL::Module::IsVR()) {
				Subtitles::HideSubtitlesHook::Install();
			} else {
				logger::info("Skipping AddMessage hook because I don't have the VR offset yet!");
			}
		}
	}
}