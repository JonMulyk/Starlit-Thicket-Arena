#include "AudioSystem.h"
#include <iostream>

void AudioSystem::init() {
	audioEngine.Init();
	std::cout << "Audio System Initialized" << std::endl;

	audioEngine.LoadBank("project/assets/audio/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL);
	audioEngine.LoadBank("project/assets/audio/Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL);
	audioEngine.LoadEvent("event:/car");

	audioEngine.LoadSound(explosionSound);

	//start playing menu music
	audioEngine.LoadSound(menuMusic);
	audioEngine.LoadSound(battleMusic);
	//play menu music at low volume
	audioEngine.PlaySounds(menuMusic, Vector3{ 0, 0, 0 }, -20.0f);

}

void AudioSystem::startBattleMusic() {
	audioEngine.StopAllChannels();
	audioEngine.PlaySounds(battleMusic, Vector3{ 0, 0, 0 }, -20.0f);
}

void AudioSystem::startEvents() {
	audioEngine.LoadEventInstanced("event:/car", "car");
}

void AudioSystem::update() {
	audioEngine.Update();
}

void AudioSystem::shutdown() {
	audioEngine.Shutdown();
}

void AudioSystem::explosion(glm::vec3 position) {
	audioEngine.PlaySounds(explosionSound, Vector3{ position.x, position.y, position.z }, 0.0f);
}