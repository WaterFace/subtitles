#include "NPCNameProvider.h"
#include "NND_API.h"

const char* NPCNameProvider::GetName(RE::Actor* actor) const
{
	if (NND) {
		if (auto name = NND->GetName(actor, NND_API::NameContext::kSubtitles); !name.empty()) {
			return name.data();
		}
	}

	if (auto xTextData = actor->extraList.GetByType<RE::ExtraTextDisplayData>(); xTextData) {
		return actor->GetDisplayFullName();
	}

	if (auto actorBase = actor->GetActorBase(); actorBase) {
		if (actorBase->shortName.size() > 0) {
			return actorBase->shortName.c_str();
		}
	}

	return actor->GetName();
}

void NPCNameProvider::RequestAPI()
{
	if (!NND) {
		NND = static_cast<NND_API::IVNND1*>(NND_API::RequestPluginAPI(NND_API::InterfaceVersion::kV1));
		if (NND) {
			logger::info("Obtained NND API - {0:x}", reinterpret_cast<uintptr_t>(NND));
		} else {
			logger::info("Failed to obtain NND API");
		}
	}
}
