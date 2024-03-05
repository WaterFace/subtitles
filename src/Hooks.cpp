#include "Hooks.h"

void Subtitles::Hooks::Install()
{
	Subtitles::UpdatePCHook::Install();
	Subtitles::AddSubtitleMessage::Install();
}