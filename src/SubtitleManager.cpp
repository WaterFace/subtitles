#include "SubtitleManager.h" 
#include "NPCNameProvider.h"

namespace Subtitles
{
	const char* SubtitleManager::GetDisplayName(RE::TESObjectREFR* ref)
	{
		if (auto actor = ref->As<RE::Actor>(); actor) {
			return NPCNameProvider::GetSingleton()->GetName(actor);
		} else {
			return ref->GetName();
		}
	}

	void SubtitleManager::AddSubtitle(RE::SubtitleInfo* info)
	{
		if (!info) {
			return;
		}

		activeSubtitles.push_back(info);
	}

	void SubtitleManager::ShowSubtitles()
	{
		auto iniSettings = RE::INISettingCollection::GetSingleton();
		float maxSubtitleDistance = iniSettings->GetSetting("fMaxSubtitleDistance:Interface")->GetFloat();
		bool showSpeakerName = iniSettings->GetSetting("bShowSubtitleSpeakerName:Interface")->GetBool();
		uint32_t speakerNameColor = iniSettings->GetSetting("iSubtitleSpeakerNameColor:Interface")->GetUInt();

		constexpr const char* speakerNameFmtString = "<font color='#{:06X}'>{}</font>: {}";
		constexpr const char* speakerNameFmtColorText = "<font color='#{:06X}'>{}</font>: <font color='#{}'>{}</font>";
		constexpr const char* noSpeakerNameFmtColorText = "<font color='#{}'>{}</font>";

		// avoid the flickering in combat issue
		std::sort(activeSubtitles.begin(), activeSubtitles.end(), [](RE::SubtitleInfo*& a, RE::SubtitleInfo*& b) {
			if (a->targetDistance < b->targetDistance)
				return true;
			if (a->targetDistance > b->targetDistance)
				return false;
			auto ptrA = a->speaker.get().get();
			auto ptrB = b->speaker.get().get();
			if (ptrA < ptrB)
				return true;
			if (ptrA > ptrB)
				return false;
			return strcmp(a->subtitle.c_str(), b->subtitle.c_str()) < 0;
		});

		std::stringstream bigSubtitle{};

		bool first = true;
		uint32_t numDisplayed = 0;
		float closest = INFINITY;
		for (auto info : activeSubtitles) {
			if (numDisplayed >= maxDisplayedSubtitles) {
				break;
			}

			// activeSubtitles is sorted by distance, so this will only happen on the first subtitle
			if (info->targetDistance < closest) {
				closest = info->targetDistance;
			}

			// `targetDistance` is the squared distance
			if (info && (info->forceDisplay || info->targetDistance < maxSubtitleDistance * maxSubtitleDistance)) {
				if (!first) {
					bigSubtitle << "<br>";
				}

				// Talking Activators don't have a name, so handle that gracefully
				auto speakerName = GetDisplayName(info->speaker.get().get());

				if (showSpeakerName && speakerName && speakerName[0]) {
					if (Configuration::GetSingleton()->dimBackgroundSubtitles) {
						// Do it this way so we can handle the case where two subtitles have the same distance
						if (info->targetDistance == closest) {
							bigSubtitle << std::format(
								speakerNameFmtColorText,
								speakerNameColor,
								speakerName,
								Configuration::GetSingleton()->foregroundSubtitleColor,
								info->subtitle.c_str());
						} else {
							bigSubtitle << std::format(
								speakerNameFmtColorText,
								speakerNameColor,
								speakerName,
								Configuration::GetSingleton()->backgroundSubtitleColor,
								info->subtitle.c_str());
						}
					} else {
						bigSubtitle << std::format(speakerNameFmtString, speakerNameColor, speakerName, info->subtitle.c_str());
					}
				} else {
					if (Configuration::GetSingleton()->dimBackgroundSubtitles) {
						if (info->targetDistance == closest) {
							bigSubtitle << std::format(
								noSpeakerNameFmtColorText,
								Configuration::GetSingleton()->foregroundSubtitleColor,
								info->subtitle.c_str());
						} else {
							bigSubtitle << std::format(
								noSpeakerNameFmtColorText,
								Configuration::GetSingleton()->backgroundSubtitleColor,
								info->subtitle.c_str());
						}
					} else {
						bigSubtitle << info->subtitle.c_str();
					}
				}
				numDisplayed += 1;

				first = false;
			}
		}

		activeSubtitles.clear();

		auto hudMenu = RE::UI::GetSingleton()->GetMenu<RE::HUDMenu>(RE::HUDMenu::MENU_NAME);
		auto root = hudMenu->GetRuntimeData().root;

		if (bigSubtitle.str().length() > 0) {
			RE::GFxValue asStr(bigSubtitle.str().c_str());
			root.Invoke("ShowSubtitle", nullptr, &asStr, 1);
		} else {
			root.Invoke("HideSubtitle", nullptr, nullptr, 0);
		}
	}
}