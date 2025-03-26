#pragma once

#include <vector>

#include "Text.h"
#include "TimeSeconds.h"
#include "GameState.h"

class UIManager
{
	private:
		std::vector<Text> uiText;
		int windowWidth;
		int windowHeight;

		void initializeUIText();

	public:
		UIManager(int windowWidth, int windowHeight);
		
		void addScoreText(GameState& gameState);
		const std::vector<Text>& getUIText() const;
		void updateUIText(TimeSeconds& timer, double roundDuration, GameState& gameState);


};
