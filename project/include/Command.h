#pragma once

#include "PxPhysicsAPI.h"

class Command {
public:
	physx::PxF32 brake = 0;
	physx::PxF32 throttle = 0.;
	physx::PxF32 steer = 0;

	void hardLeft() {
		physx::PxF32 brake = 0.;
		physx::PxF32 throttle = 1.;
		physx::PxF32 steer = 1.;
	}

	void softleft() {
		physx::PxF32 brake = 0.;
		physx::PxF32 throttle = 1.;
		physx::PxF32 steer = 0.25;
	}

	void left() {
		physx::PxF32 brake = 0.;
		physx::PxF32 throttle = 1.;
		physx::PxF32 steer = 0.5;
	}

	void softRight() {
		physx::PxF32 brake = 0.;
		physx::PxF32 throttle = 1.;
		physx::PxF32 steer = -.25;
	}

	void hardRight() {
		physx::PxF32 brake = 0.;
		physx::PxF32 throttle = 1.;
		physx::PxF32 steer = -1.;
	}

	void right() {
		physx::PxF32 brake = 0.;
		physx::PxF32 throttle = 1.;
		physx::PxF32 steer = -0.5;
	}

	void forward() {
		physx::PxF32 brake = 0.;
		physx::PxF32 throttle = 1.;
		physx::PxF32 steer = -0.5;
	}
};

