#pragma once

#include "PxPhysicsAPI.h"

class Command {
public:
	physx::PxF32 brake = 0;
	physx::PxF32 throttle = 1.;
	physx::PxF32 steer = 0;
	physx::PxIntBool boost = false;
	physx::PxF32 fuel = 2.f;

	void updateBoost(float dt) {
		// have max 2 seconds of boost
		if (boost) {
			fuel -= dt;
		}
		// take 3 times as long to recharge
		else {
			fuel += dt/3;
		}

		// clamp fuel
		fuel = physx::PxClamp(fuel, 0.f, 2.f);
	}

	void reset() {
		fuel = 2.f;
	}
};

