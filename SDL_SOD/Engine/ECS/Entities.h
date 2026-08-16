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
	EntityID ID = 0;
	std::vector<Component*> components;

	Entity() 
	{
		components.reserve(10);
		componentsIndex.reserve(10);
	};

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
		if (componentsIndex.find(typeid(value)) != componentsIndex.end())
			return static_cast<T*>(components[componentsIndex[typeid(value)]]);

		T* component = new T(value);
		Push_Back(component);
		return component;
	}
	template <ValidComponent T>
	T* AddComponent()
	{
		if (componentsIndex.find(typeid(T)) != componentsIndex.end())
			return static_cast<T*>(components[componentsIndex[typeid(T)]]);

		T* component = new T();
		Push_Back(component);
		return component;
	}
	template <ValidComponent T>
	bool RemoveComponent()
	{
		if (componentsIndex.find(typeid(T)) != componentsIndex.end()) 
		{
			size_t index = componentsIndex[typeid(T)];
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
		if (componentsIndex.find(typeid(T)) != componentsIndex.end())
			return static_cast<T*>(components[componentsIndex[typeid(T)]]);
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
	EntityID IDManager = 0;
	struct ParsedObject
	{
		std::string name{ "T" };
		std::string textureName{};
		Vec2f pos;
	};
	[[nodiscard]] std::vector<ParsedObject> LoadObjectFile(const std::string& path);

public:
	std::vector<Entity*> entities;

	Entity& CreateEntity(Entity& prefab);
	Entity& CreateEntity(Entity* prefab);
	Entity& CreateEntity();

	void CreateEntitiesFromObj(const std::string& path, const std::string& prefabName, Entity& prefab);
	void DestroyEntity(Entity& entity);
};