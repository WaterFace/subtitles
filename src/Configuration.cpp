#include "Configuration.h"

namespace Subtitles
{
	void Configuration::Initialize()
	{
		CSimpleIniA ini;
		ini.SetUnicode();

		auto iniError = ini.LoadFile(PATH.data());

		if (iniError < 0) {
			SKSE::stl::report_and_fail(std::format("Failed to read {}.ini", Plugin::NAME));
		}

		numDisplayedSubtitles = ini.GetLongValue("Subtitles", "numDisplayedSubtitles", 4);
		dimBackgroundSubtitles = ini.GetBoolValue("Subtitles", "dimBackgroundSubtitles", false);
		foregroundSubtitleColor = std::string(ini.GetValue("Subtitles", "foregroundSubtitleColor", "FFFFFF"));
		backgroundSubtitleColor = std::string(ini.GetValue("Subtitles", "backgroundSubtitleColor", "999999"));

		logger::info("Configuration initialized");
	}
}