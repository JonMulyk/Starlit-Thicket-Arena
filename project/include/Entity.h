#pragma once

#include "Transform.h"
#include "Model.h"
#include "Vehicle.h"
#include <string>

class Entity
{
public:
	std::string name;
	Model& model;
	Transform* transform;
	Vehicle* vehicle = nullptr;

	Entity(std::string name, Model& model, Transform* transform);

/*
	~Entity() {
		if (transform != nullptr) {
			delete transform;
			transform = nullptr;
		}
		if (vehicle != nullptr) {
			vehicle->vehicle.destroy();
			vehicle = nullptr;
		}
	}
*/
};