#include "ConsoleCommands.h"

namespace ShowAnimCommand
{
	bool ConsoleCommands::Exec(const RE::SCRIPT_PARAMETER*, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData, RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR*, RE::Script*, RE::ScriptLocals*, double&, std::uint32_t&)
	{
		auto ToClipGenerator = [](RE::hkbNode* a_node) -> RE::hkbClipGenerator* {
			constexpr char CLASS_NAME[] = "hkbClipGenerator";
			if (a_node && a_node->GetClassType()) {
				if (_strcmpi(a_node->GetClassType()->name, CLASS_NAME) == 0)
					return skyrim_cast<RE::hkbClipGenerator*>(a_node);
			}

			return nullptr;
		};

		auto actor = a_thisObj ? a_thisObj->As<RE::Actor>() : nullptr;
		if (actor) {
			RE::BSAnimationGraphManagerPtr graphMgr;
			if (actor->GetAnimationGraphManager(graphMgr) && graphMgr) {
				auto behaviourGraph = graphMgr->graphs[0] ? graphMgr->graphs[0]->behaviorGraph : nullptr;
				auto activeNodes = behaviourGraph ? behaviourGraph->activeNodes : nullptr;
				if (activeNodes) {
					for (auto nodeInfo : *activeNodes) {
						auto nodeClone = nodeInfo.nodeClone;
						if (nodeClone && nodeClone->GetClassType()) {
							auto clipGenrator = ToClipGenerator(nodeClone);
							if (clipGenrator)
								CPrint("Get an Active Anim: \"%s\", localTime: %f", clipGenrator->animationName, clipGenrator->localTime);
						}
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

			logger::debug("Replace ShowAnim Console Command Successfully!");
		} else {
			logger::error("Fail to Locate ShowAnim Console Command!");
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
