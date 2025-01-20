#include <iostream>

#include "PxPhysicsAPI.h"
#include "PhysicsSystem.h"
#include "Entity.h"

int main()
{
	PhysicsSystem physicsSys;

	// std::cout << physicsSys.rigidDynamicList.size() << std::endl;

	// Simulate at 60fps

	std::vector<Entity> entityList;
	entityList.reserve(465);

	for (int i = 0; i < 465; i++)
	{
		entityList.emplace_back();
		entityList.back().name = "box";
		entityList.back().transform = physicsSys.transformList[i];
		entityList.back().model = NULL;
	}

	while (1)
	{
		physicsSys.gScene->simulate(1.0f / 60.0f);
		physicsSys.gScene->fetchResults(true);
		physicsSys.updateTransforms();

		physx::PxVec3 objPos = physicsSys.getPos(50);
		std::cout << "x: " << objPos.x << " y: " << objPos.y << " z: " << objPos.z << std::endl;
		std::cout << entityList[50].transform->pos.y << std::endl;
	}

	return 0;
}