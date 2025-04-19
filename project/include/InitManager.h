#pragma once
#include <vector>
#include <GLFW/glfw3.h>


class InitManager {
public:
	static void initGLFW();
	static void getCube(std::vector<float>& vert, std::vector<float>& coord);
	static void getGround(std::vector<float>& vert, std::vector<float>& norms, std::vector<float>& coord);
};
