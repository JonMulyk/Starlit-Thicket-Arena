#pragma once

#include <vector>

#include "Text.h"
#include "TimeSeconds.h"

class UIManager
{
	private:
		std::vector<Text> uiText;
		int windowWidth;
		int windowHeight;

		void initializeUIText();

	public:
		UIManager(int windowWidth, int windowHeight);
		
		const std::vector<Text>& getUIText() const;
		void updateUIText(TimeSeconds& timer, double roundDuration, uint64_t score);


};
