#include <vector>

namespace Subtitles
{
	// struct SubtitleData
	// {
	// 	RE::ObjectRefHandle speaker;
	// 	RE::BSString subtitle;
	// 	float distance;
	// };

	class SubtitleManager
	{
	public:
		static SubtitleManager* GetSingleton()
		{
			static SubtitleManager sm;
			return &sm;
		}

		static const char* GetDisplayName(RE::TESObjectREFR* ref);

		void AddSubtitle(RE::SubtitleInfo* info);
		void ShowSubtitles();

		std::vector<RE::SubtitleInfo*> activeSubtitles;

	protected:
		SubtitleManager() = default;
		~SubtitleManager() = default;

		SubtitleManager(const SubtitleManager&) = delete;
		SubtitleManager(SubtitleManager&&) = delete;
		SubtitleManager& operator=(const SubtitleManager&) = delete;
		SubtitleManager& operator=(SubtitleManager&&) = delete;
	};
}