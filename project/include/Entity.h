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

    // Constructor
    Entity(std::string name, Model& model, Transform* transform);

    // Destructor: free owned resources
    ~Entity() {
        delete transform;
        delete vehicle;
    }

    // Delete copy operations to prevent accidental copying
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    // Move constructor: transfer ownership of pointers
    Entity(Entity&& other) noexcept
        : name(std::move(other.name)),   // move the name string
        model(other.model),              // copy the reference (cannot be moved)
        transform(other.transform),      // transfer pointer ownership
        vehicle(other.vehicle)           // transfer pointer ownership
    {
        // Null out the source pointers to avoid double deletion
        other.transform = nullptr;
        other.vehicle = nullptr;
    }

    // Move assignment operator: clean up and transfer ownership
    Entity& operator=(Entity&& other) noexcept {
        if (this != &other) {
            // Clean up current resources
            delete transform;
            delete vehicle;

            // Move data from other to this object
            name = std::move(other.name);
            // model is a reference; it's assumed both entities refer to the same model.
            // if last element
            transform = other.transform;
            vehicle = other.vehicle;

            // Null out other's pointers to prevent double deletion
            other.transform = nullptr;
            other.vehicle = nullptr;
        }
        return *this;
    }

};