#pragma once

#ifndef _AUDIO_ENGINE_H_
#define _AUDIO_ENGINE_H_

#include "fmod_studio.hpp"
#include "fmod.hpp"
#include <string>
#include <map>
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

struct Vector3 {
    float x;
    float y;
    float z;
};

struct Implementation {
    Implementation();
    ~Implementation();

    void Update();

    FMOD::Studio::System* mpStudioSystem;
    FMOD::System* mpSystem;

    int mnNextChannelId;  // Make sure to initialize this (for example, to 0) in your constructor.

    typedef map<string, FMOD::Sound*> SoundMap;
    typedef map<int, FMOD::Channel*> ChannelMap;
    typedef map<string, FMOD::Studio::EventInstance*> EventMap;
    typedef map<string, FMOD::Studio::Bank*> BankMap;
    BankMap mBanks;
    EventMap mEvents;
    SoundMap mSounds;
    ChannelMap mChannels;
};

class CAudioEngine {
public:
    // Static functions for initialization, update, shutdown, and error checking.
    static void Init();
    static void Update();
    static void Shutdown();
    static int ErrorCheck(FMOD_RESULT result);

    // Sound and bank functions.
    void LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
    void LoadEvent(const std::string& strEventName);
    void LoadEventInstanced(const string& strEventName, const string& strEventInstanceName);
    void LoadSound(const std::string& strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
    void UnLoadSound(const std::string& strSoundName);

    // Listener and channel functions.
    void Set3dListenerAndOrientation(const Vector3& vPosition, const Vector3& vLook, const Vector3& vUp);
    int PlaySounds(const std::string& strSoundName, const Vector3& vPos = Vector3{ 0, 0, 0 }, float fVolumedB = 0.0f);
    // New signature with an output channel parameter:
    int PlaySounds(const std::string& soundFile, Vector3 position, float volume, FMOD::Channel** channel);
    void StopChannel(int nChannelId);
    void StopAllChannels();
    bool IsPlaying(int nChannelId) const;
    void SetChannel3dPosition(int nChannelId, const Vector3& vPosition);
    void SetChannelVolume(int nChannelId, float fVolumedB);

    // Event functions.
    void PlayEvent(const std::string& strEventName);
    void StopEvent(const std::string& strEventName, bool bImmediate = false);
    void GetEventParameter(const std::string& strEventName, const std::string& strParameterName, float* parameter);
    void SetEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue);
    bool IsEventPlaying(const std::string& strEventName) const;

    // Utility functions.
    float dbToVolume(float dB);
    float VolumeTodB(float volume);
    FMOD_VECTOR VectorToFmod(const Vector3& vPosition);
};

#endif
