#pragma once

#include "PxPhysicsAPI.h"

class Command {
public:
	physx::PxF32 brake = 0;
	physx::PxF32 throttle = 1.;
	physx::PxF32 steer = 0;

};

