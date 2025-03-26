#pragma once

#include "PxPhysicsAPI.h"
#include "Transform.h"
#include "Model.h"
#include <string>

class Vehicle;

class Entity
{
public:
	std::string name;
	Model* model;
	Transform* transform;
	Vehicle* vehicle = nullptr;
	physx::PxVec2 start;
	physx::PxVec2 end;

    // Constructor
    Entity(std::string name, Model* model, Transform* transform);
};