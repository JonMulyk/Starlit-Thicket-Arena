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
		void addScoreText(GameState& gameState);
		void initializeFuelBars(int numberOfBars, Shader& fuelBarShader, GameState& gameState);

	public:
		UIManager(int windowWidth, int windowHeight);

		void initializeUIElements(GameState& gameState, Shader& fuelBarShader);
		
		const std::vector<Text>& getUIText() const;
		void updateUIText(TimeSeconds& timer, double roundDuration, GameState& gameState, int player);

		std::vector<FuelBar>& getFuelBars();
		void resetFuel(GameState& gameState);
};
