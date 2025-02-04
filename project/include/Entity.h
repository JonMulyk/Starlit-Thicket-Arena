#pragma once
#pragma once

#include "Transform.h"
#include "Model.h"
#include <string>

class Entity
{
public:
	std::string name;
	Model& model;
	Transform* transform;

	Entity(std::string name, Model& model, Transform* transform);
};