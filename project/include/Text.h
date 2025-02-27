#pragma once


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <string>

class Text
{
	private:
		const std::string name;
		std::string textToRender;
		float x;
		float y;
		float scale;
		glm::vec3 color;

	public:
		Text(const std::string& name, const std::string& text, float x, float y, float scale, const glm::vec3& color);
		Text(const std::string& text, float x, float y, float scale, const glm::vec3& color);
	
		// getters and setters
		const std::string& getName() const;

		const std::string& getTextToRender() const;
		void setTextToRender(std::string text);

		float getX() const;
		float getY() const;
		float getScale() const;
		glm::vec3 getColor() const;

};
