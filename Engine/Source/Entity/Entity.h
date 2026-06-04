// ============================================================
// Ultimate Source Engine - Entity
// ============================================================
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include "Component.h"

namespace USE
{
	class Entity
	{
	public:
		Entity(const std::string& name = "Untitled");
		~Entity();

		// Name
		void SetName(const std::string& name) { m_name = name; }
		const std::string& GetName() const { return m_name; }

		// Component management
		template <typename T, typename... Args>
		T* AddComponent(Args&&... args)
		{
			static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
			auto it = m_components.find(std::type_index(typeid(T)));
			if (it != m_components.end()) return static_cast<T*>(it->second.get());

			auto component = std::make_unique<T>(std::forward<Args>(args)...);
			component->SetOwner(this);
			T* ptr = component.get();
			m_components[std::type_index(typeid(T))] = std::move(component);
			return ptr;
		}

		template <typename T>
		T* GetComponent()
		{
			auto it = m_components.find(std::type_index(typeid(T)));
			return (it != m_components.end()) ? static_cast<T*>(it->second.get()) : nullptr;
		}

		template <typename T>
		const T* GetComponent() const
		{
			auto it = m_components.find(std::type_index(typeid(T)));
			return (it != m_components.end()) ? static_cast<const T*>(it->second.get()) : nullptr;
		}

		template <typename T>
		bool HasComponent() const { return m_components.find(std::type_index(typeid(T))) != m_components.end(); }

		template <typename T>
		T* GetOrAddComponent()
		{
			auto* comp = GetComponent<T>();
			if (comp) return comp;
			return AddComponent<T>();
		}

		// Hierarchy
		void SetParent(Entity* parent);
		Entity* GetParent() const { return m_parent; }
		void AddChild(Entity* child);
		void RemoveChild(Entity* child);
		const std::vector<Entity*>& GetChildren() const { return m_children; }
		uint32_t GetChildrenCount() const { return static_cast<uint32_t>(m_children.size()); }

	private:
		std::string m_name;
		Entity* m_parent = nullptr;
		std::vector<Entity*> m_children;
		std::unordered_map<std::type_index, std::unique_ptr<Component>> m_components;
	};
}