#include "ConsoleCommands.h"
#include "OpenAnimationReplacer/API/OpenAnimationReplacerAPI-Animations.h"

namespace ShowAnimCommand
{
	bool ConsoleCommands::Exec(const RE::SCRIPT_PARAMETER*, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData, RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR*, RE::Script*, RE::ScriptLocals*, double&, std::uint32_t&)
	{
		using namespace OAR_API::Animations;

		auto ToClipGenerator = [](RE::hkbNode* a_node) -> RE::hkbClipGenerator* {
			if (a_node && a_node->GetClassType()) {
				if (_strcmpi(a_node->GetClassType()->name, "hkbClipGenerator") == 0)
					return skyrim_cast<RE::hkbClipGenerator*>(a_node);

				if (_strcmpi(a_node->GetClassType()->name, "BSSynchronizedClipGenerator") == 0) {
					auto syncClip = skyrim_cast<RE::BSSynchronizedClipGenerator*>(a_node);
					if (syncClip)
						return syncClip->clipGenerator;
				}
			}

			return nullptr;
		};

		if (a_thisObj) {
			RE::BSAnimationGraphManagerPtr graphMgr;
			if (a_thisObj->GetAnimationGraphManager(graphMgr) && graphMgr) {
				for (const auto project : graphMgr->graphs) {
					auto behaviourGraph = project ? project->behaviorGraph : nullptr;
					auto activeNodes = behaviourGraph ? behaviourGraph->activeNodes : nullptr;
					if (activeNodes) {
						CPrint("Behavior Project Name :  \"%s\"", project->projectName.c_str());
						auto API = GetAPI();
						for (auto nodeInfo : *activeNodes) {
							auto nodeClone = nodeInfo.nodeClone;
							if (nodeClone && nodeClone->GetClassType()) {
								auto clipGenrator = ToClipGenerator(nodeClone);
								if (clipGenrator)
									if (API) {
										auto animInfo = API->GetCurrentReplacementAnimationInfo(clipGenrator);
										auto animPath = animInfo.variantFilename.empty() ? std::string(animInfo.animationPath.c_str()) :
										                                                   std::string(animInfo.animationPath.c_str()) + "\\" + std::string(animInfo.variantFilename.c_str());

										std::string clipPath = animPath.empty() ? clipGenrator->animationName.c_str() : animPath.c_str();
										CPrint("Get an Active Anim: \"%s\", localTime: %f", clipPath.c_str(), clipGenrator->localTime);
									} else {
										CPrint("Get an Active Anim: \"%s\", localTime: %f", clipGenrator->animationName, clipGenrator->localTime);
									}
							}
						}
						CPrint("---------------------------------------");
					}
				}
			}
		}

		return true;
	}

	void ConsoleCommands::Register()
	{
		using Type = RE::SCRIPT_PARAM_TYPE;

		auto info = RE::SCRIPT_FUNCTION::LocateConsoleCommand("ShowAnim");  // Unused
		if (info) {
			info->referenceFunction = true;
			info->executeFunction = Exec;
			info->conditionFunction = nullptr;
			info->editorFilter = false;
			info->invalidatesCellList = false;

			DEBUG("Replace ShowAnim Console Command Successfully!");
		} else {
			ERROR("Fail to Locate ShowAnim Console Command!");
		}
	}

	void ConsoleCommands::CPrint(const char* a_fmt, ...)
	{
		auto console = RE::ConsoleLog::GetSingleton();
		if (console && console->IsConsoleMode()) {
			std::va_list args;
			va_start(args, a_fmt);
			console->VPrint(a_fmt, args);
			va_end(args);
		}
	}

}
