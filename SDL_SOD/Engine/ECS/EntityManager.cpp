#include "Entities.h"
#include "../Macros/DEBUGPRINT.h"

#include <filesystem>
#include <sstream>
#include <string>
#include <fstream>

// --- EntityManager --- 

Entity& EntityManager::CreateEntity(Entity* prefab)
{
	IDManager++;
	entities.emplace_back(new Entity(*prefab))->ID = IDManager;
	return *entities.back();
}

Entity& EntityManager::CreateEntity(Entity& prefab)
{
	IDManager++;
	entities.emplace_back(new Entity(prefab))->ID = IDManager;
	return *entities.back();
}

Entity& EntityManager::CreateEntity()
{
	IDManager++;
	entities.emplace_back(new Entity())->ID = IDManager;
	return *entities.back();
}

void SwapAndPop(std::vector<Entity*>& entities, int index)
{
	Entity* temp = entities[index];
	entities[index] = entities.back();
	entities.back() = temp;

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

		if(DEBUGPRINT)
			std::cout << "[" << "\033[32m" << "ENTITYMANAGER" << "\033[37m" << "] " << "Destroying entity : " << &entity << "\n";

		delete entities[i];
		SwapAndPop(entities, i);
		break;
	}
}

std::string Trim(std::string str)
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
				obj.pos.x = x + 85.0f;
				obj.pos.y = y + 178.0f;
			}

			std::getline(file, line);
			if (line.starts_with("Texture"))
			{
				size_t divider = line.find(":");
				obj.textureName = Trim(line.substr(divider + 1));
			}

			objects.push_back(obj);
		}
	}

	return objects;
}

void EntityManager::CreateEntitiesFromObjFile(const std::string& path, const std::string& prefabName, Entity& prefab) 
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

			if(DEBUGPRINT)
				std::cout << obj.pos << "\n";
			transform->position = obj.pos;
		}
	}
}