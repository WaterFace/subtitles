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

	class AddSubtitleMessage
	{
	public:
		static void Install()
		{
			AddMessage = RE::Offset::UIMessageQueue::AddMessage;
		}

	private:
		static void AddMessageMod(RE::UIMessageQueue* queue, RE::BSFixedString* menuName, RE::UI_MESSAGE_TYPE type, RE::HUDData* data)
		{
			// Don't pass through messages that lead to the `ShowSubtitles` branch. We'll handle those
			if (!data || data->type != RE::HUDData::Type::kSubtitle) {
				AddMessage(queue, menuName, type, data);
			}
		}
		static inline REL::Relocation<decltype(AddMessageMod)> AddMessage;
	};

}