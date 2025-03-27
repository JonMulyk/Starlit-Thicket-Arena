#pragma once

#include "PxPhysicsAPI.h"

class Command {
public:
	physx::PxF32 brake = 0;
	physx::PxF32 throttle = 1.;
	physx::PxF32 steer = 0;
	physx::PxIntBool boost = false;
	physx::PxF32 fuel = 1.f;

	void updateBoost(float dt) {
		// have max 2 seconds of boost
		if (boost) {
			fuel -= dt;
		}
		// take 6 times as long to recharge
		else {
			fuel += dt/6;
		}

		// clamp fuel
		fuel = physx::PxClamp(fuel, 0.f, 1.f);
	}

	void reset() {
		fuel = 1.f;
	}
};

