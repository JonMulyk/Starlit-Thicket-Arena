#pragma once

#include <vector>

#include "Text.h"
#include "TimeSeconds.h"
#include "GameState.h"
#include "FuelBar.h"

class UIManager
{
	private:
		std::vector<Text> uiText;
		std::vector<FuelBar> fuelBars;
		int windowWidth;
		int windowHeight;
		float textScale = 1.0f;
		bool fuelTextEnabled = false;


		void initializeUIText();

	public:
		UIManager(int windowWidth, int windowHeight);
		
		void addScoreText(GameState& gameState);
		const std::vector<Text>& getUIText() const;
		void updateUIText(TimeSeconds& timer, double roundDuration, GameState& gameState, int player);

		void initializeFuelBars(int numberOfBars, Shader& fuelBarShader, GameState& gameState);
		std::vector<FuelBar>& getFuelBars();
		void resetFuel(GameState& gameState);
};
