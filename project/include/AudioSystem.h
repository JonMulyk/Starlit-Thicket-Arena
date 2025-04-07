#pragma once

#include "AudioEngine.h"
#include "PhysicsSystem.h"
#include "Camera.h"
#include <string>
#include <glm/glm.hpp>
#include <iostream>



class AudioSystem {
public:
	void init();
	void init(PhysicsSystem* physicsSystem, Camera* camera);
	void startEvents();
	void update();
	void shutdown();





	void startBattleMusic();

	void explosion(glm::vec3 position);
	void startCar();

	void playAISound(glm::vec3 position);

	void startMenuMusic();

	void startLevelMusic();

	void stopMusic();

	void stopCarSounds();
	void stopAISounds();
	//stop car sound at a specific position
	void stopAISounds(glm::vec3 position);

	void startCarSounds();
	void startAISounds();

	bool carSoundPlaying = false;

	CAudioEngine audioEngine;
	CAudioEngine* audioEnginePtr;
	PhysicsSystem* c_physicsSystem;
	Camera* c_camera;
	bool carPlaying = false;
	float pitchAdjust = 2.0f;
	float carVolume = -10.0f;
	float aiVolume = -5.0f;
	float musicVolume = -30.0f;
	float explosionVolume = 0.0f;

	std::string menuMusic = "project/assets/audio/menuMusic.wav";
	std::string battleMusic = "project/assets/audio/battleMusic.wav";
	std::string explosionSound = "project/assets/audio/explosion.wav";
	std::string carSound = "project/assets/audio/futureCycle.wav";
	std::string AISound = "project/assets/audio/AIVehicle.wav";

	FMOD::Channel* carChannel = nullptr;
	FMOD::Channel* musicChannel = nullptr;
	std::vector<FMOD::Channel*> aiChannels;
};