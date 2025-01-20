#include <iostream>

#include "PxPhysicsAPI.h"
#include "PhysicsSystem.h"

int main()
{
	PhysicsSystem physicsSys;

	std::cout << physicsSys.rigidDynamicList.size();

	// Simulate at 60fps
	while (1)
	{
		physicsSys.gScene->simulate(1.0f / 60.0f);
		physicsSys.gScene->fetchResults(true);

		physx::PxVec3 objPos = physicsSys.getPos(50);
		std::cout << "x: " << objPos.x << "y: " << objPos.y << "z: " << objPos.z << std::endl;
	}

	return 0;
}