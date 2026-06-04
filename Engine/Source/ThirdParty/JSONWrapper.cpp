#include "stdafx.h"
#include "JSONWrapper.h"
#include "Core/Logger.h"

// #define USE_NLOHMANN_JSON   // nlohmann/json single header
// #define USE_RAPIDJSON       // alternative

#if defined(USE_NLOHMANN_JSON)
#include <nlohmann/json.hpp>
#elif defined(USE_RAPIDJSON)
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#endif

namespace USE
{
	bool JSONWrapper::Parse(const std::string& text, JsonValue& outRoot)
	{
#if defined(USE_NLOHMANN_JSON)
		try {
			auto j = nlohmann::json::parse(text);
			// Convert to our internal JsonValue (omitted)
			return true;
		}
		catch (...) {
			USE_LOG_ERROR("JSONWrapper: Failed to parse JSON.");
			return false;
		}
#elif defined(USE_RAPIDJSON)
		rapidjson::Document doc;
		doc.Parse(text.c_str());
		if (doc.HasParseError()) {
			USE_LOG_ERROR("JSONWrapper: Parse error at offset %zu", doc.GetErrorOffset());
			return false;
		}
		// Convert to internal format
		return true;
#else
		USE_LOG_WARN("JSONWrapper: No JSON library enabled. Define USE_NLOHMANN_JSON or USE_RAPIDJSON.");
		return false;
#endif
	}
}