#pragma once

#include "Transform.h"
#include "Model.h"
#include <string>

class Vehicle;

class Entity
{
public:
	std::string name;
	Model& model;
	Transform* transform;
	Vehicle* vehicle = nullptr;

	Entity(std::string name, Model& model, Transform* transform);
};