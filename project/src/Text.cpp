#include "Text.h"

Text::Text(const std::string& name, const std::string& text, float x, float y, float scale, const glm::vec3& color)
	: name(name), textToRender(text), x(x), y(y), scale(scale), color(color) 
{
}

Text::Text(const std::string& name, const std::string& text, float x, float y, float scale, const glm::vec3& color, TTF::TEXT_POSITION textPosition)
	: name(name), textToRender(text), x(x), y(y), scale(scale), color(color), textPosition(textPosition)
{
}


Text::Text(const std::string& text, float x, float y, float scale, const glm::vec3& color)
	: name(""), textToRender(text), x(x), y(y), scale(scale), color(color) 
{
}

Text::Text(const std::string& text, float x, float y, float scale, const glm::vec3& color, TTF::TEXT_POSITION textPosition)
	: name(""), textToRender(text), x(x), y(y), scale(scale), color(color), textPosition(textPosition)
{
}

Text::Text(const Text& other)
	: name(other.name), textToRender(other.textToRender), x(other.x), y(other.y), scale(other.scale), color(other.color), textPosition(other.textPosition)
{
}

Text& Text::operator=(const Text& other)
{
	if (this != &other)
	{
		//name = other.name;
		textToRender = other.textToRender;
		x = other.x;
		y = other.y;
		scale = other.scale;
		color = other.color;
		textPosition = other.textPosition;
	}

	return *this;
}


const std::string& Text::getName() const
{
	return this->name;
}

const std::string& Text::getTextToRender() const
{
	return this->textToRender;
}

void Text::setTextToRender(std::string text)
{
	this->textToRender = text;
}

float Text::getX() const
{
	return this->x;
}

float Text::getY() const
{
	return this->y;
}

float Text::getScale() const
{
	return this->scale;
}

glm::vec3 Text::getColor() const
{
	return this->color;
}

TTF::TEXT_POSITION Text::getTextPosition() const
{
	return this->textPosition;
}
