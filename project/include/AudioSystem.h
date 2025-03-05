#pragma once

#include "AudioEngine.h"
#include <string>
#include <glm/glm.hpp>
#include <iostream>


class AudioSystem {
public:
	void init();
	void startEvents();
	void update();
	void shutdown();

	void startBattleMusic();

	void explosion(glm::vec3 position);

	CAudioEngine audioEngine;
	CAudioEngine* audioEnginePtr;

	std::string menuMusic = "project/assets/audio/menuMusic.wav";
	std::string battleMusic = "project/assets/audio/battleMusic.wav";
	std::string explosionSound = "project/assets/audio/explosion.wav";

};