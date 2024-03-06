#include "SubtitleManager.h"

namespace Subtitles
{
	namespace Hooks
	{
		void Install();
	}

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

	// Hook the place where the RE::SubtitleManager sends the message containing the subtitle to the UI
	class AddSubtitleMessage
	{
	public:
		static void Install()
		{
			// TODO: get VR address and offset
			auto address = REL::VariantID(51761, 52637, 0).address();
			auto offset = REL::VariantOffset(0x1A2, 0x18E, 0).offset();
			AddMessage = SKSE::GetTrampoline().write_call<5>(address + offset, AddMessageMod);
			logger::info("UIMessageQueue::AddMessage hooked");
		}

	private:
		static void AddMessageMod(RE::UIMessageQueue* queue, RE::BSFixedString* menuName, RE::UI_MESSAGE_TYPE type, RE::HUDData* data)
		{
			if (!data || data->type != RE::HUDData::Type::kSubtitle) {
				AddMessage(queue, menuName, type, data);
			}
		}
		static inline REL::Relocation<decltype(AddMessageMod)> AddMessage;
	};

}