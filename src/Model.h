#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 col;
};

class Model
{
	public:
		std::vector<Vertex> verts;
		glm::mat4 modelMatrix;
};