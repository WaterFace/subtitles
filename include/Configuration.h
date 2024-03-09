#pragma once

#include <string_view>

namespace Subtitles
{
	class Configuration
	{
	public:
		static Configuration* GetSingleton()
		{
			static Configuration config{};
			return &config;
		}

		void Initialize();

		inline static const std::string PATH = std::format("Data/SKSE/Plugins/{}.ini", Plugin::NAME);

		// Settings
		uint32_t numDisplayedSubtitles;
		bool dimBackgroundSubtitles;
		std::string backgroundSubtitleColor;
		std::string foregroundSubtitleColor;

	protected:
		Configuration() = default;
		~Configuration() = default;

		Configuration(const Configuration&) = delete;
		Configuration(Configuration&&) = delete;
		Configuration& operator=(const Configuration&) = delete;
		Configuration& operator=(Configuration&&) = delete;
	};
}