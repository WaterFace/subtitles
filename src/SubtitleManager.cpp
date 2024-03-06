#include "SubtitleManager.h"

namespace Subtitles
{
	const char* SubtitleManager::GetDisplayName(RE::TESObjectREFR* ref)
	{
		auto actor = ref ? ref->As<RE::Actor>() : nullptr;
		auto xTextData = actor ? actor->extraList.GetByType<RE::ExtraTextDisplayData>() : nullptr;

		if (xTextData) {
			return xTextData->displayName.c_str();
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

		std::stringstream bigSubtitle{};

		bool first = true;
		uint32_t numDisplayed = 0;
		for (auto info : activeSubtitles) {
			if (numDisplayed >= maxDisplayedSubtitles) {
				break;
			}

			// `targetDistance` is the squared distance
			if (info && info->targetDistance < maxSubtitleDistance * maxSubtitleDistance) {
				if (!first) {
					bigSubtitle << "<br>";
				}
				if (showSpeakerName) {
					auto speakerName = GetDisplayName(info->speaker.get().get());
					bigSubtitle << std::format(speakerNameFmtString, speakerNameColor, speakerName, info->subtitle.c_str());
				} else {
					bigSubtitle << info->subtitle.c_str();
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