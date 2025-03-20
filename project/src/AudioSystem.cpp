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

	//audioEngine.PlaySounds(menuMusic, Vector3{ 0, 0, 0 }, musicVolume);
	startMenuMusic();
}

void AudioSystem::init(PhysicsSystem* physicsSystem, Camera* camera) {
	c_physicsSystem = physicsSystem;
	c_camera = camera;
	audioEngine.Init();
	std::cout << "Audio System Initialized" << std::endl;

	audioEngine.LoadBank("project/assets/audio/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL);
	audioEngine.LoadBank("project/assets/audio/Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL);

	audioEngine.LoadSound(explosionSound);
	audioEngine.LoadSound(carSound);
	audioEngine.LoadSound(menuMusic);
	audioEngine.LoadSound(battleMusic);

	//audioEngine.PlaySounds(menuMusic, Vector3{ 0, 0, 0 }, musicVolume);
	startBattleMusic();
}

void AudioSystem::startBattleMusic() {
	//audioEngine.StopAllChannels();
	startCar();
	carPlaying = true;
	audioEngine.PlaySounds(battleMusic, Vector3{ 0, 0, 0 }, musicVolume, &musicChannel);
	//music is not 3d and loop
	musicChannel->setMode(FMOD_LOOP_NORMAL | FMOD_2D);
	musicChannel->setLoopCount(-1);


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
	// Update listener attributes
	// Replace these dummy values with your actual listener (e.g., camera) data.
	Vector3 listenerPosition = { 0.0f, 0.0f, 0.0f };
	Vector3 listenerLook = { 0.0f, 0.0f, 1.0f };
	Vector3 listenerUp = { 0.0f, 1.0f, 0.0f };
	audioEngine.Set3dListenerAndOrientation(listenerPosition, listenerLook, listenerUp);

	// Update the car sound pitch if the physics system is available.
	if (carPlaying) {
		float carSpeed = c_physicsSystem->getCarSpeed(0);
		float engineRPM = c_physicsSystem->calculateEngineRPM(carSpeed);

		// Map engine RPM to a pitch multiplier.
		// For example, if 800 RPM is idle (pitch 1.0) and 7000 RPM is max.
		float pitch = 1.0f + pitchAdjust * (engineRPM - 800.0f) / (7000.0f - 800.0f);
		// Update the car sound channel with the new pitch.
		carChannel->setPitch(pitch);

		c_physicsSystem->getPos(0);
		physx::PxVec3 pos = c_physicsSystem->getPos(0);

		//set pos of carChannel
		FMOD_VECTOR posFMOD;
		posFMOD.x = pos.x;
		posFMOD.y = pos.y;
		posFMOD.z = pos.z;
		carChannel->set3DAttributes(&posFMOD, 0);

		//get explosion at explosion location
		if (c_physicsSystem->getExplosion())
			explosion(c_physicsSystem->getExplosionLocation());

		//listener attributes
		listenerPosition = { pos.x, pos.y, pos.z };
		listenerLook = { 0.0f, 0.0f, 1.0f };
		listenerUp = { 0.0f, 1.0f, 0.0f };
		audioEngine.Set3dListenerAndOrientation(listenerPosition, listenerLook, listenerUp);
		
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

		//pitch adjust
		float aiSpeed = c_physicsSystem->getCarSpeed(i + 1);
		float aiEngineRPM = c_physicsSystem->calculateEngineRPM(aiSpeed);
		float aiPitch = 1.0f + pitchAdjust * (aiEngineRPM - 800.0f) / (7000.0f - 800.0f);

		aiChannels[i]->setPitch(aiPitch);
	}
	
}

void AudioSystem::shutdown() {
	audioEngine.Shutdown();
}



void AudioSystem::explosion(glm::vec3 position) {
	audioEngine.PlaySounds(explosionSound, Vector3{ position.x, position.y, position.z }, explosionVolume);
}

void AudioSystem::startCar() {
	audioEngine.PlaySounds(carSound, Vector3{ 0, 0, 0 }, carVolume, &carChannel);
	// Set the channel to loop in 3D mode:
	carChannel->setMode(FMOD_LOOP_NORMAL | FMOD_3D);
	carChannel->setLoopCount(-1);  // -1 means infinite looping.
}


// New function: play a 3D sound from an AI opponent.
void AudioSystem::playAISound(glm::vec3 position) {
	FMOD::Channel* aiChannel = nullptr;
	int result = audioEngine.PlaySounds(AISound, Vector3{ position.x, position.y, position.z }, aiVolume, &aiChannel);
	aiChannel->setMode(FMOD_LOOP_NORMAL | FMOD_3D);
	aiChannel->set3DMinMaxDistance(1.0f, 1000.0f);
	aiChannel->setLoopCount(-1);  // -1 means infinite looping.
	aiChannels.push_back(aiChannel);
	//std::cout << "Playing AI sound at position (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
	//if (result != FMOD_OK || aiChannel == nullptr) {
	//	std::cout << "Error playing AI sound at position ("
	//		<< position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
	//}
	//else {
	//	// Set the channel to 3D mode so that it is positioned in the world.
	//	
	//}
}


void AudioSystem::startMenuMusic() {
	if (musicChannel == nullptr) {
		// If the music channel is null, create and initialize it
		audioEngine.PlaySounds(menuMusic, Vector3{ 0, 0, 0 }, musicVolume, &musicChannel);
		musicChannel->setMode(FMOD_LOOP_NORMAL | FMOD_2D);  // 2D for non-3D sound
		musicChannel->setLoopCount(-1);  // Infinite looping
	}
}

void AudioSystem::stopMusic() {
	if (musicChannel != nullptr) {
		musicChannel->stop();  // Stop the music channel
		musicChannel = nullptr;  // Set to nullptr after stopping
	}
}
