#pragma once
#include <vector>
#include "Components.h"
#include <iostream>
#include "../Macros/DEBUGPRINT.h"
#include <typeindex>
#include <unordered_map>

template<typename T>
concept ValidComponent = std::is_base_of_v<Component, T>;

class Entity
{
private:
	void DestroyComponents() {
		if (DEBUGPRINT) {
			std::cout << "~Entity " << this
				<< " id=" << ID
				<< std::endl;
		}
		for (auto comp : components)
		{
			if (DEBUGPRINT)
				std::cout << " deleting " << comp << std::endl;
			delete comp;
		}
		components.clear();
	}
	void Push(Component* comp);

	std::unordered_map<std::type_index, size_t> componentsIndex;
public: 
	std::vector<Component*> components;
	size_t ID = 0;

	Entity()
	{
		components.reserve(10);
		componentsIndex.reserve(10);
	}

	template<ValidComponent T>
	inline void Push_Back(T* component)
	{
		Component* comp = static_cast<Component*>(component);
		Push(comp);
		componentsIndex.emplace(typeid(*component), components.size() - 1);
	}

	template <ValidComponent T>
	T* AddComponent(const T& value)
	{
		auto it = componentsIndex.find(typeid(value));
		if (it != componentsIndex.end())
		{
			return static_cast<T*>(components[it->second]);
		}

		T* component = new T(value);
		Push_Back(component);
		return component;
	}
	template <ValidComponent T>
	T* AddComponent()
	{
		auto it = componentsIndex.find(typeid(T));
		if (it != componentsIndex.end())
		{
			return static_cast<T*>(components[it->second]);
		}

		T* component = new T();
		Push_Back(component);
		return component;
	}
	template <ValidComponent T>
	bool RemoveComponent()
	{
		auto it = componentsIndex.find(typeid(T));
		if (it != componentsIndex.end()) 
		{
			size_t index = it->second;
			delete components[index];
			components[index] = components.back();
			components.pop_back();
		}
		return false;
	}
	template <ValidComponent T>
	bool HasComponent() const
	{
		if (componentsIndex.find(typeid(T)) != componentsIndex.end())
			return true;
		return false;
	}
	template <ValidComponent T>
	T* GetComponent()
	{
		auto it = componentsIndex.find(typeid(T));
		if (it != componentsIndex.end()) 
		{
			return static_cast<T*>(components[it->second]);
		}
		return nullptr;
	}
	~Entity()
	{
		DestroyComponents();
	}

	Entity(Entity&& other) noexcept
		: components(std::move(other.components)),
		ID(other.ID)
	{
		other.ID = -1;
		for (Component* c : components)
			c->parent = this;

		if constexpr (DEBUGPRINT)
			std::cout << "[" << "\033[35m" << "ENTITY" << "\033[37m" << "] : Created " << this << "\n";
	}
	Entity(const Entity& other)
	{
		ID = other.ID;

		// Create copies
		for (auto* component : other.components)
		{
			if (!component)
				continue;

			Component* clone = component->Clone();
			Push_Back(clone);
		}

		if constexpr (DEBUGPRINT)
			std::cout << "[" << "\033[35m" << "ENTITY" << "\033[37m" << "] : Created " << this << "\n";
	}

	Entity& operator=(Entity&& other) noexcept
	{
		if (this == &other)
			return *this;

		// Free any memory already in use
		DestroyComponents();

		ID = other.ID;
		other.ID = -1;

		// Move the pointers
		this->components = std::move(other.components);
		other.components.clear();

		for (Component* c : components)
			c->parent = this;

		return *this;
	}
	Entity& operator=(const Entity& other) 
	{
		if (&other == this)
			return *this;

		ID = other.ID;

		// Free any memory already in use
		DestroyComponents();

		// Create copies
		for (Component* component : other.components)
		{
			if (!component)
				continue;

			Component* clone = component->Clone();
			Push_Back(clone);
		}
		return *this;
	}
};

class EntityManager
{
private:
	size_t IDManager = 0;
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
	}
	std::vector<Entity*> entities;

	inline Entity& CreateEntity(Entity* prefab)
	{
		IDManager++;
		entities.emplace_back(new Entity(*prefab))->ID = IDManager;
		return *entities.back();
	}
	inline Entity& CreateEntity(Entity& prefab)
	{
		IDManager++;
		entities.emplace_back(new Entity(prefab))->ID = IDManager;
		return *entities.back();
	}
	inline Entity& CreateEntity()
	{
		IDManager++;
		entities.emplace_back(new Entity())->ID = IDManager;
		return *entities.back();
	}

	void CreateEntitiesFromObj(const std::string& path, const std::string& prefabName, Entity& prefab);

	void DestroyEntity(Entity& entity);
};