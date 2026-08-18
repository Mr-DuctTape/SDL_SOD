#include "Entities.h"
#include "../Macros/DEBUGPRINT.h"

#include <filesystem>
#include <sstream>
#include <string>
#include <fstream>

void Entity::Push(Component* comp)
{
	comp->parent = this;
	comp->Init();
	components.push_back(comp);
}

// --- EntityManager --- 

void SwapAndPop(std::vector<Entity*>& entities, int index)
{
	if constexpr(DEBUGPRINT)
		std::cout << "[" << "\033[32m" << "ENTITYMANAGER" << "\033[37m" << "] " << "Destroying entity : " << &entities[index] << "\n";

	delete entities[index];
	entities[index] = std::move(entities.back());
	entities.pop_back();
}

void EntityManager::DestroyEntity(Entity& entity)
{
	for (size_t i = 0; i < entities.size(); )
	{
		if (entities[i]->ID != entity.ID)
		{
			i++;
			continue;
		}

		SwapAndPop(entities, i);
		break;
	}
}

static std::string Trim(std::string str)
{
	str.erase(0, str.find_first_not_of(" \t"));
	str.erase(str.find_last_not_of(" \t") + 1);
	return str;
}

std::vector<EntityManager::ParsedObject> EntityManager::LoadObjectFile(const std::string& path)
{
	std::vector<EntityManager::ParsedObject> objects;
	std::filesystem::path p{ path };
	if (!std::filesystem::exists(p)) {
		std::cout << "Path doesn't exist or is invalid: " << path << "\n";
		return objects;
	}

	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cout << "Invalid path: " << path << "\n";
		return objects;
	}

	std::string line;

	constexpr float offsetX = 85.0f;
	constexpr float offsetY = 178.0f;

	//Load objects
	while (std::getline(file, line)) 
	{
		if (line.starts_with("Object"))
		{
			ParsedObject obj;
			std::getline(file, line);
			if (line.starts_with("Name"))
			{
				size_t divider = line.find(":");
				obj.name = Trim(line.substr(divider + 1));
			}

			std::getline(file, line);
			if (line.starts_with("Pos"))
			{
				size_t divider = line.find(":");
				std::string values = Trim(line.substr(divider + 1));
				std::stringstream ss(values);
				int x, y;
				ss >> x >> y;
				obj.pos.x = x + offsetX;
				obj.pos.y = y + offsetY;
			}

			objects.push_back(obj);
		}
	}

	return objects;
}

void EntityManager::CreateEntitiesFromObj(const std::string& path, const std::string& prefabName, Entity& prefab) 
{
	std::vector<ParsedObject> objects = LoadObjectFile(path);

	for (ParsedObject& obj : objects)
	{
		if (obj.name == prefabName)
		{
			// Create the entitiy
			Entity& entity = CreateEntity(&prefab);
			Transform* transform = entity.GetComponent<Transform>();
			if (!transform)
				continue;

			transform->position = obj.pos;
		}
	}
}