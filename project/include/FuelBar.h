#pragma once
#include "Shader.h"
#include "Model.h"
#include <memory>

class FuelBar
{
	public:
		FuelBar(Shader& shader, float x, float y, 
			float width, float height);

		void updateFuelBar(float fuelLevel);

		Model& getFuelBarModel();

	private:
		std::unique_ptr<Model> fuelBarModel;
		float fuelLevel;
		float x;
		float y;
		float width;
		float height;
		glm::vec4 color;

		void updateFuelBarSize();
		void updateFuelBarColor();
};
