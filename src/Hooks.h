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

			auto subtitleManager = RE::SubtitleManager::GetSingleton();
			logger::info("{} subtitles currently active:", subtitleManager->subtitles.size());
			for (auto& subtitleInfo : subtitleManager->subtitles) {
				std::string speakerName;
				if (auto speaker = subtitleInfo.speaker.get()) {
					speakerName = speaker->GetName();
				}
				logger::info("\t{}: {}", speakerName, subtitleInfo.subtitle.c_str());
			}
		}
		static inline REL::Relocation<decltype(UpdatePCMod)> UpdatePC;
	};

}