#include "AudioSystem.h"
#include <iostream>

void AudioSystem::init() {
	audioEngine.Init();
	std::cout << "Audio System Initialized" << std::endl;

	audioEngine.LoadBank("project/assets/audio/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL);
	audioEngine.LoadBank("project/assets/audio/Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL);

	audioEngine.LoadSound(explosionSound);
	audioEngine.LoadSound(carSound);
	audioEngine.LoadSound(menuMusic);
	audioEngine.LoadSound(battleMusic);

	audioEngine.PlaySounds(menuMusic, Vector3{ 0, 0, 0 }, -20.0f);
}

void AudioSystem::init(PhysicsSystem* physicsSystem) {
	c_physicsSystem = physicsSystem;
	audioEngine.Init();
	std::cout << "Audio System Initialized" << std::endl;

	audioEngine.LoadBank("project/assets/audio/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL);
	audioEngine.LoadBank("project/assets/audio/Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL);

	audioEngine.LoadSound(explosionSound);
	audioEngine.LoadSound(carSound);
	audioEngine.LoadSound(menuMusic);
	audioEngine.LoadSound(battleMusic);

	audioEngine.PlaySounds(menuMusic, Vector3{ 0, 0, 0 }, -20.0f);
}

void AudioSystem::startBattleMusic() {
	audioEngine.StopAllChannels();
	startCar();
	carPlaying = true;
	audioEngine.PlaySounds(battleMusic, Vector3{ 0, 0, 0 }, -20.0f);
}

void AudioSystem::startEvents() {
	audioEngine.LoadEventInstanced("event:/car", "car");
}

void AudioSystem::update() {
	audioEngine.Update();
	// Update the car sound pitch if the physics system is available.
	if (carPlaying) {
		// Get the current car speed from the physics system.
		float carSpeed = c_physicsSystem->getCarSpeed();
		// Calculate engine RPM based on the car's speed.
		float engineRPM = c_physicsSystem->calculateEngineRPM(carSpeed);

		// Map engine RPM to a pitch multiplier.
		// For example, if 800 RPM is idle (pitch 1.0) and 7000 RPM is max.
		float pitch = 1.0f + pitchAdjust * (engineRPM - 800.0f) / (7000.0f - 800.0f);
		// Update the car sound channel with the new pitch.
		carChannel->setPitch(pitch);
	}
}

void AudioSystem::shutdown() {
	audioEngine.Shutdown();
}

void AudioSystem::explosion(glm::vec3 position) {
	audioEngine.PlaySounds(explosionSound, Vector3{ position.x, position.y, position.z }, 0.0f);
}

void AudioSystem::startCar() {
	audioEngine.PlaySounds(carSound, Vector3{ 0, 0, 0 }, 0, &carChannel);
	carChannel->setMode(FMOD_LOOP_NORMAL);
}