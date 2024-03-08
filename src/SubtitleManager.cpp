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

		// if the new subtitle has the same distance as the previous one
		// insert it so the two are sorted by their pointers, to avoid flickering
		if (activeSubtitles.size() > 0) {
			auto prev = activeSubtitles[activeSubtitles.size() - 1];

			if (prev->targetDistance == info->targetDistance) {
				auto ptr0 = prev->speaker.get().get();
				auto ptr1 = info->speaker.get().get();
				if (ptr0 > ptr1) {
					auto it = activeSubtitles.begin() + activeSubtitles.size() - 1;
					activeSubtitles.insert(it, info);
					return;
				}
			}
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

				// Talking Activators don't have a name, so handle that gracefully
				auto speakerName = GetDisplayName(info->speaker.get().get());

				if (showSpeakerName && speakerName && speakerName[0]) {
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