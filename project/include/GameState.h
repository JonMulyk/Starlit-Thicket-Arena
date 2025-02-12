#pragma once
#include <vector>
#include "Entity.h"
class GameState {
public:
	std::vector<Entity> dynamicEntities;
	std::vector<Entity> staticEntities;
};