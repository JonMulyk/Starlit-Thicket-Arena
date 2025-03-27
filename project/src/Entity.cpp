#pragma once
#include "Entity.h"

Entity::Entity(std::string name, Model* model, Transform* transform)
	: name(name), model(model), transform(transform)
{}

void Entity::setScale(float x, float y, float z) {
    if (transform) {
        transform->scale = glm::vec3(x, y, z);
    }
}