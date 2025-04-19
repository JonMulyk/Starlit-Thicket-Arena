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
		Shader& fuelBarShader;
		float textScale = 1.0f;
		bool fuelTextEnabled = false;

		void initializeUIText();
		void addScoreText(GameState& gameState);
		void initializeFuelBars(int numberOfBars, GameState& gameState);

	public:
		UIManager(int windowWidth, int windowHeight, Shader& fuelBarShader);

		void initializeUIElements(GameState& gameState);
		
		const std::vector<Text>& getUIText() const;
		void updateUIText(TimeSeconds& timer, double roundDuration, GameState& gameState, int player);

		std::vector<FuelBar>& getFuelBars();
		void resetFuel(GameState& gameState);
};
