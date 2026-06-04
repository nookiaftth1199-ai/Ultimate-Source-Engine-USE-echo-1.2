#pragma once

#include "Component.h"
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <memory>

namespace USE
{
	class ComponentManager
	{
	public:
		ComponentManager() = default;
		~ComponentManager() = default;

		// Register a component type (optional – for future use).
		template <typename T>
		void RegisterComponentType()
		{
			m_componentTypes[std::type_index(typeid(T))] = {};
		}

		// Retrieve all components of a specific type (for systems).
		template <typename T>
		std::vector<T*> GetComponents()
		{
			// This is a simple implementation; in a real ECS you'd store them per type.
			// For now, systems will iterate over entities manually.
			return {};
		}

	private:
		std::unordered_map<std::type_index, bool> m_componentTypes;
	};
}