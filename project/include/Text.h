#pragma once


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <string>
#include "TTF.h"

class Text
{
	private:
		const std::string name;
		std::string textToRender;
		float x;
		float y;
		float scale;
		glm::vec3 color;
		TTF::TEXT_POSITION textPosition = TTF::TEXT_POSITION::CENTER;

	public:
		Text(const std::string& name, const std::string& text, float x, float y, float scale, const glm::vec3& color);
		Text(const std::string& name, const std::string& text, float x, float y, float scale, const glm::vec3& color, TTF::TEXT_POSITION textPosition);
		Text(const std::string& text, float x, float y, float scale, const glm::vec3& color);
		Text(const std::string& text, float x, float y, float scale, const glm::vec3& color, TTF::TEXT_POSITION textPosition);

		//copy constructor
		Text(const Text& other);
		// copy assignment operator
		Text& operator=(const Text& other);

	
		// getters and setters
		const std::string& getName() const;

		const std::string& getTextToRender() const;
		void setTextToRender(std::string text);

		float getX() const;
		float getY() const;
		float getScale() const;
		glm::vec3 getColor() const;
		TTF::TEXT_POSITION getTextPosition() const;

};
