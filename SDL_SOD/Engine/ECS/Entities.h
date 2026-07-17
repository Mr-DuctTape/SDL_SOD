#pragma once
#include <vector>
#include "Components.h"
#include <iostream>
#include "../Macros/DEBUGPRINT.h"

class Entity
{
public:
	EntityID ID = 0;
	std::vector<Component*> components{};

	Entity() {};
	Entity(const Entity& other)
	{
		ID = other.ID;
		for (auto* component : other.components)
		{
			if (!component)
			{
				components.push_back(nullptr);
				continue;
			}

			Component* clone = component->Clone();
			Push_Back(clone);
		}
	}
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
		if (DEBUGPRINT) {
			std::cout << "~Entity " << this
				<< " id=" << ID
				<< std::endl;
		}

		for (auto comp : components)
		{
			if(DEBUGPRINT)
				std::cout << " deleting " << comp << std::endl;
			delete comp;
		}
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

		bool operator== (const ParsedObject& other) const
		{
			return (pos == other.pos && name == other.name && textureName == other.textureName);
		}
	};
	std::vector<ParsedObject> LoadObjectFile(const std::string& path);
public:
	std::vector<Entity*> entities;
	Entity& CreateEntity(Entity& prefab);
	Entity* CreateEntity(Entity* prefab);
	Entity& CreateEntity();


	void CreateEntitiesFromObjFile(const std::string& path, const std::string& prefabName, Entity& prefab);
	void DestroyEntity(Entity& entity);
	~EntityManager();
};