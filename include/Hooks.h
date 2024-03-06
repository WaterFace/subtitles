#include "IUIMessageData.h"
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
			auto address = REL::VariantID(0, 52637, 0).address();    // TODO: get SE and VR addresses
			auto offset = REL::VariantOffset(0, 0x18E, 0).offset();  // TODO: etc.
			AddMessage = SKSE::GetTrampoline().write_call<5>(address + offset, AddMessageMod);
			logger::info("UIMessageQueue::AddMessage hooked");
		}

	private:
		static void AddMessageMod(RE::UIMessageQueue* queue, RE::BSFixedString* menuName, RE::UI_MESSAGE_TYPE type, RE::IUIMessageData* data)
		{
			// do nothing on purpose
			auto messageData = reinterpret_cast<Subtitle::IUIMessageData*>(data);
			logger::trace("payload: {}, type: {}", messageData->payload.c_str(), messageData->type);
			if (messageData->type != Subtitle::MessageType::kShowSubtitle) {
				AddMessage(queue, menuName, type, data);
			}
		}
		static inline REL::Relocation<decltype(AddMessageMod)> AddMessage;
	};

}