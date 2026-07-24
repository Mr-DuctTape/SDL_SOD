#pragma once
#include <vector>
#include "Components.h"
#include <iostream>
#include "../Macros/DEBUGPRINT.h"

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
public:
	EntityID ID = 0;
	std::vector<Component*> components{};

	Entity() {};
	inline void Push_Back(Component* component)
	{
		component->parent = this;
		component->Init();
		components.push_back(component);
	}
	template <typename T>
	T* AddComponent(const T& value)
	{
		static_assert(std::is_base_of_v<Component, T>, "T must be a component!");
		for (size_t i = 0; i < components.size(); i++)
		{
			if (T* component = dynamic_cast<T*>(components[i]))
			{
				return component;
			}
		}
		T* component = new T(value);
		Push_Back(component);
		return component;
	}
	template <typename T>
	T* AddComponent()
	{
		static_assert(std::is_base_of_v<Component, T>, "T must be a component!");
		for (size_t i = 0; i < components.size(); i++)
		{
			if (T* component = dynamic_cast<T*>(components[i]))
			{
				return component;
			}
		}
		T* component = new T();
		Push_Back(component);
		return component;
	}
	template <typename T>
	bool RemoveComponent()
	{
		for (size_t i = 0; i < components.size(); i++)
		{
			if (T* component = dynamic_cast<T*>(components[i]))
			{
				delete components[i];
				components.erase(components.begin() + i);
				return true;
			}
		}
		return false;
	}
	template <typename T>
	bool HasComponent() const
	{
		static_assert(std::is_base_of_v<Component, T>, "T must be a component!");
		for (size_t i = 0; i < components.size(); i++)
		{
			if (T* component = dynamic_cast<T*>(components[i]))
			{
				return true;
			}
		}
		return false;
	}
	template <typename T>
	T* GetComponent()
	{
		static_assert(std::is_base_of_v<Component, T>, "T must be a component!");
		for (size_t i = 0; i < components.size(); i++)
		{
			if (T* component = dynamic_cast<T*>(components[i]))
			{
				return component;
			}
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
	std::vector<ParsedObject> LoadObjectFile(const std::string& path);

public:
	std::vector<Entity*> entities;

	Entity& CreateEntity(Entity& prefab);
	Entity& CreateEntity(Entity* prefab);
	Entity& CreateEntity();

	void CreateEntitiesFromObjFile(const std::string& path, const std::string& prefabName, Entity& prefab);
	void DestroyEntity(Entity& entity);
};