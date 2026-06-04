#pragma once
#include "../Component.h"
#include <string>

namespace USE
{
	class ScriptComponent : public Component
	{
	public:
		void LoadScript(const std::string& path) { m_scriptPath = path; }
		const std::string& GetScriptPath() const { return m_scriptPath; }

	private:
		std::string m_scriptPath;
	};
}