#include "FuelBar.h"

FuelBar::FuelBar(Shader& shader, float x, float y, float width, float height)
	: fuelLevel(1.0f), x(x), y(y), width(width), height(height), color(glm::vec4(0.0f))
{
    this->fuelBarModel = std::make_unique<Model>((Model::createRectangleModel(shader, -0.9f, -0.95f, 0.4f, 0.05f)));
}

void FuelBar::updateFuelBar(float fuelLevel)
{
    this->fuelLevel = fuelLevel;

    updateFuelBarSize();
    updateFuelBarColor();
}

Model& FuelBar::getFuelBarModel()
{
    return *this->fuelBarModel;
}

void FuelBar::updateFuelBarSize()
{
    float clamped = std::min(std::max(fuelLevel, 0.0f), 1.0f);
    float currentWidth = width * clamped;

    std::vector<float> vertices = {
		x, y, 0.0f,
		x + currentWidth, y, 0.0f,
		x + currentWidth, y + height, 0.0f,

		x, y, 0.0f,
		x + currentWidth, y + height, 0.0f,
		x, y + height, 0.0f
    };

    this->fuelBarModel->updateVertices(vertices);
}

void FuelBar::updateFuelBarColor()
{
    float fuelLevelGreenMin = 0.999f;
    float fuelLevelYellowMin = 0.375f;
    float fuelLevelRedMin = 0.0f;

    glm::vec4 green = { 32.0f / 255.0f, 237.0f / 255.0f, 34.0f / 255.0f, 1.0f };
    glm::vec4 yellow = {226.0f / 255.0f, 182.0f / 255.0f, 26.0f / 255.0f, 1.0f};
    glm::vec4 red = { 255.0f / 255.0f, 21.0f / 255.0f, 15.0f / 255.0f, 1.0f };

    if (fuelLevel > fuelLevelGreenMin)
    {
        this->color = green;
    }
    else if (fuelLevel > fuelLevelYellowMin)
    {
        this->color = yellow;
    }
    else
    {
        this->color = red;
    }

	this->fuelBarModel->getShader().use();
    this->fuelBarModel->getShader().setVec4("barColor", color);
}
