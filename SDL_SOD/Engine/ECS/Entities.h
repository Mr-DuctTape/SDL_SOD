#pragma once
#include <vector>
#include "Components.h"
#include <iostream>
#include "../Macros/DEBUGPRINT.h"
#include <typeindex>
#include <unordered_map>
#include <optional>

template<typename T>
concept ValidComponent = std::is_base_of_v<Component, T>;

class Entity
{
private:
	void DestroyComponents() {
		if (DEBUGPRINT) {
			std::cout << "~Entity " << this
				<< " id=" << m_id
				<< std::endl;
		}
		for (auto comp : m_components)
		{
			if (DEBUGPRINT)
				std::cout << " deleting " << comp << std::endl;
			delete comp;
		}
		m_components.clear();
	}
	void Emplace(Component* comp);

	template<ValidComponent T>
	inline void Emplace_Back(T* component)
	{
		Component* comp = static_cast<Component*>(component);
		Emplace(comp);
		m_componentsIndex.emplace(typeid(*component), m_components.size() - 1);
	}

	std::unordered_map<std::type_index, size_t> m_componentsIndex;

public: 
	std::vector<Component*> m_components;
	size_t m_id = 0;

	template <ValidComponent T>
	T* AddComponent(const T& value)
	{
		auto it = m_componentsIndex.find(typeid(value));
		if (it != m_componentsIndex.end())
		{
			return static_cast<T*>(m_components[it->second]);
		}

		T* component = new T(value);
		Emplace_Back(component);
		return component;
	}
	template <ValidComponent T>
	T* AddComponent()
	{
		auto it = m_componentsIndex.find(typeid(T));
		if (it != m_componentsIndex.end())
		{
			return static_cast<T*>(m_components[it->second]);
		}

		T* component = new T();
		Emplace_Back(component);
		return component;
	}
	template <ValidComponent T>
	bool RemoveComponent()
	{
		auto it = m_componentsIndex.find(typeid(T));
		if (it != m_componentsIndex.end()) 
		{
			size_t index = it->second;
			delete m_components[index];
			m_components[index] = m_components.back();
			m_components.pop_back();
			m_componentsIndex.erase(it);
			return true;
		}
		return false;
	}
	template <ValidComponent T>
	bool HasComponent() const
	{
		if (m_componentsIndex.find(typeid(T)) != m_componentsIndex.end())
			return true;
		return false;
	}
	template <ValidComponent T>
	T& GetComponent()
	{
		return *static_cast<T*>(m_components[m_componentsIndex[typeid(T)]]);
	}
	template <ValidComponent T>
	T* FindComponent()
	{
		auto it = m_componentsIndex.find(typeid(T));
		if (it != m_componentsIndex.end())
		{
			return static_cast<T*>(m_components[it->second]);
		}
		return nullptr;
	}

	~Entity()
	{
		DestroyComponents();
	}

	Entity() {}

	Entity(Entity&& other) noexcept 
		: m_components(std::move(other.m_components)),
		m_id(other.m_id)
	{
		other.m_id = -1;
		for (Component* c : m_components) {
			c->parent = this;
		}

		if constexpr (DEBUGPRINT)
			std::cout << "[" << "\033[35m" << "ENTITY" << "\033[37m" << "] : Created " << this << "\n";
	}
	Entity(const Entity& other) // Copy construct
	{
		// Create copies
		for (auto* component : other.m_components)
		{
			if (!component)
				continue;

			Component* clone = component->Clone();
			Emplace_Back(clone);
		}

		if constexpr (DEBUGPRINT)
			std::cout << "[" << "\033[35m" << "ENTITY" << "\033[37m" << "] : Created " << this << "\n";
	}

	Entity& operator=(Entity&& other) noexcept // Move assigment
	{
		if (this == &other)
			return *this;

		// Free any memory already in use
		DestroyComponents();

		m_id = other.m_id;
		other.m_id = -1;

		// Move the pointers
		this->m_components = std::move(other.m_components);
		other.m_components.clear();

		for (Component* c : m_components)
			c->parent = this;

		return *this;
	}
	Entity& operator=(const Entity& other)  // Copy assigment
	{
		if (&other == this)
			return *this;

		m_id = other.m_id;

		// Free any memory already in use
		DestroyComponents();

		// Create copies
		for (Component* component : other.m_components)
		{
			if (!component)
				continue;

			Component* clone = component->Clone();
			Emplace_Back(clone);
		}
		return *this;
	}
};

class EntityManager
{
private:
	size_t m_idManager = 0;
	struct ParsedObject
	{
		std::string name{ "T" };
		Vec2f pos;
	};
	[[nodiscard]] std::vector<ParsedObject> LoadObjectFile(const std::string& path);

public:
	EntityManager()
	{
		if constexpr (DEBUGPRINT)
			std::cout << "[" << "\033[32m" << "ENTITYMANAGER" << "\033[37m" << "] : Created" << "\n";

		constexpr int memory_reserved = 128;
		entities.reserve(memory_reserved);
	}
	std::vector<Entity*> entities;

	inline Entity& CreateEntity(Entity* prefab)
	{
		m_idManager++;
		entities.emplace_back(new Entity(*prefab))->m_id = m_idManager;
		return *entities.back();
	}
	inline Entity& CreateEntity(Entity& prefab)
	{
		m_idManager++;
		entities.emplace_back(new Entity(prefab))->m_id = m_idManager;
		return *entities.back();
	}
	inline Entity& CreateEntity()
	{
		m_idManager++;
		entities.emplace_back(new Entity())->m_id = m_idManager;
		return *entities.back();
	}

	void CreateEntitiesFromObj(const std::string& path, const std::string& prefabName, Entity& prefab);

	void DestroyEntity(Entity& entity);
};