#pragma once
#include "Entity.h"

Entity::Entity(std::string name, Model* model, Transform* transform)
	: name(name), model(model), transform(transform), start(0), end(0)
{}