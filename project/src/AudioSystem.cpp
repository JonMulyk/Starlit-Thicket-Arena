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

	audioEngine.PlaySounds(menuMusic, Vector3{ 0, 0, 0 }, -35.0f);
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

	audioEngine.PlaySounds(menuMusic, Vector3{ 0, 0, 0 }, -35.0f);
}

void AudioSystem::startBattleMusic() {
	audioEngine.StopAllChannels();
	startCar();
	carPlaying = true;
	audioEngine.PlaySounds(battleMusic, Vector3{ 0, 0, 0 }, -35.0f);
	// Set the car sound channel to 3D mode so that it is positioned in the world.
	carChannel->setMode(FMOD_3D);

	//start AI sounds
	std::vector<physx::PxVec3> aiPositions = c_physicsSystem->getAIPositions();
	for (size_t i = 0; i < aiPositions.size(); i++) {
		playAISound(glm::vec3(aiPositions[i].x, aiPositions[i].y, aiPositions[i].z));
	}
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
	
		// Update 3D attributes of AI sounds.
		
	}
	std::vector<physx::PxVec3> aiPositions = c_physicsSystem->getAIPositions();
	size_t numChannels = std::min(aiChannels.size(), aiPositions.size());
	for (size_t i = 0; i < numChannels; i++) {
		FMOD_VECTOR pos;
		pos.x = aiPositions[i].x;
		pos.y = aiPositions[i].y;
		pos.z = aiPositions[i].z;
		FMOD_VECTOR vel = { 0, 0, 0 };
		aiChannels[i]->set3DAttributes(&pos, &vel);
	}
}

void AudioSystem::shutdown() {
	audioEngine.Shutdown();
}

void AudioSystem::explosion(glm::vec3 position) {
	audioEngine.PlaySounds(explosionSound, Vector3{ position.x, position.y, position.z }, -25.0f);
}

void AudioSystem::startCar() {
	//audioEngine.PlaySounds(carSound, Vector3{ 0, 0, 0 }, -30.0f, &carChannel);
	//carChannel->setMode(FMOD_LOOP_NORMAL);
}

// New function: play a 3D sound from an AI opponent.
void AudioSystem::playAISound(glm::vec3 position) {
	FMOD::Channel* aiChannel = nullptr;
	aiChannel->setMode(FMOD_LOOP_NORMAL);
	int result = audioEngine.PlaySounds(AISound, Vector3{ position.x, position.y, position.z }, 0.0f, &aiChannel);
	std::cout << "Playing AI sound at position (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
	if (result != FMOD_OK || aiChannel == nullptr) {
		std::cout << "Error playing AI sound at position ("
			<< position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
	}
	else {
		// Set the channel to 3D mode so that it is positioned in the world.
		aiChannel->setMode(FMOD_3D);
		// Optionally, store the channel if you need to update its 3D attributes continuously.
		aiChannels.push_back(aiChannel);
	}
}