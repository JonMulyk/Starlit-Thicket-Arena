#pragma once
#include <vector>
#include "Entity.h"
class GameState {
public:
	std::vector<Entity> dynamicList;
	std::vector<Entity> staticList;
};