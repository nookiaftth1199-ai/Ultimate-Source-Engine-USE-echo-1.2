#pragma once

#include <vector>
#include <memory>
#include "SystemManager.h"

namespace USE
{
	class SystemBase
	{
	public:
		virtual ~SystemBase() = default;
		virtual void Update(float deltaTime) = 0;
	};

	class SystemManager
	{
	public:
		SystemManager() = default;
		~SystemManager() = default;

		template <typename T, typename... Args>
		T* RegisterSystem(Args&&... args)
		{
			static_assert(std::is_base_of<SystemBase, T>::value, "T must derive from SystemBase");
			auto system = std::make_unique<T>(std::forward<Args>(args)...);
			T* ptr = system.get();
			m_systems.push_back(std::move(system));
			return ptr;
		}

		void Update(float deltaTime)
		{
			for (auto& system : m_systems)
				system->Update(deltaTime);
		}

	private:
		std::vector<std::unique_ptr<SystemBase>> m_systems;
	};
}