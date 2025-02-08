#include "AudioEngine.h"

Implementation::Implementation() {
    mpStudioSystem = NULL;
    CAudioEngine::ErrorCheck(FMOD::Studio::System::create(&mpStudioSystem));
    CAudioEngine::ErrorCheck(mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));

    mpSystem = NULL;
    // Use getCoreSystem instead of getLowLevelSystem
    CAudioEngine::ErrorCheck(mpStudioSystem->getCoreSystem(&mpSystem));
}

Implementation::~Implementation() {
    CAudioEngine::ErrorCheck(mpStudioSystem->unloadAll());
    CAudioEngine::ErrorCheck(mpStudioSystem->release());
}

void Implementation::Update() {
    vector<ChannelMap::iterator> pStoppedChannels;
    for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it)
    {
        bool bIsPlaying = false;
        it->second->isPlaying(&bIsPlaying);
        if (!bIsPlaying)
        {
            pStoppedChannels.push_back(it);
        }
    }
    for (auto& it : pStoppedChannels)
    {
        mChannels.erase(it);
    }
    CAudioEngine::ErrorCheck(mpStudioSystem->update());
}

Implementation* sgpImplementation = nullptr;

void CAudioEngine::Init() {
    sgpImplementation = new Implementation;
}

void CAudioEngine::Update() {
    sgpImplementation->Update();
}

void CAudioEngine::LoadSound(const std::string& strSoundName, bool b3d, bool bLooping, bool bStream)
{
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    if (tFoundIt != sgpImplementation->mSounds.end())
        return;
    FMOD_MODE eMode = FMOD_DEFAULT;
    eMode |= b3d ? FMOD_3D : FMOD_2D;
    eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;
    FMOD::Sound* pSound = nullptr;
    CAudioEngine::ErrorCheck(sgpImplementation->mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));
    if (pSound) {
        sgpImplementation->mSounds[strSoundName] = pSound;
    }
}

void CAudioEngine::UnLoadSound(const std::string& strSoundName)
{
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    if (tFoundIt == sgpImplementation->mSounds.end())
        return;
    CAudioEngine::ErrorCheck(tFoundIt->second->release());
    sgpImplementation->mSounds.erase(tFoundIt);
}

int CAudioEngine::PlaySounds(const string& strSoundName, const Vector3& vPosition, float fVolumedB)
{
    int nChannelId = sgpImplementation->mnNextChannelId++;
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    if (tFoundIt == sgpImplementation->mSounds.end())
    {
        LoadSound(strSoundName);
        tFoundIt = sgpImplementation->mSounds.find(strSoundName);
        if (tFoundIt == sgpImplementation->mSounds.end())
        {
            return nChannelId;
        }
    }
    FMOD::Channel* pChannel = nullptr;
    CAudioEngine::ErrorCheck(sgpImplementation->mpSystem->playSound(tFoundIt->second, nullptr, true, &pChannel));
    if (pChannel)
    {
        FMOD_MODE currMode;
        tFoundIt->second->getMode(&currMode);
        if (currMode & FMOD_3D) {
            FMOD_VECTOR position = VectorToFmod(vPosition);
            CAudioEngine::ErrorCheck(pChannel->set3DAttributes(&position, nullptr));
        }
        CAudioEngine::ErrorCheck(pChannel->setVolume(dbToVolume(fVolumedB)));
        CAudioEngine::ErrorCheck(pChannel->setPaused(false));
        sgpImplementation->mChannels[nChannelId] = pChannel;
    }
    return nChannelId;
}

void CAudioEngine::SetChannel3dPosition(int nChannelId, const Vector3& vPosition)
{
    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->mChannels.end())
        return;

    FMOD_VECTOR position = VectorToFmod(vPosition);
    CAudioEngine::ErrorCheck(tFoundIt->second->set3DAttributes(&position, NULL));
}

void CAudioEngine::SetChannelVolume(int nChannelId, float fVolumedB)
{
    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->mChannels.end())
        return;

    CAudioEngine::ErrorCheck(tFoundIt->second->setVolume(dbToVolume(fVolumedB)));
}

void CAudioEngine::LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags) {
    auto tFoundIt = sgpImplementation->mBanks.find(strBankName);
    if (tFoundIt != sgpImplementation->mBanks.end())
        return;
    FMOD::Studio::Bank* pBank;
    CAudioEngine::ErrorCheck(sgpImplementation->mpStudioSystem->loadBankFile(strBankName.c_str(), flags, &pBank));
    if (pBank) {
        sgpImplementation->mBanks[strBankName] = pBank;
    }
}

void CAudioEngine::LoadEvent(const std::string& strEventName) {
    auto tFoundit = sgpImplementation->mEvents.find(strEventName);
    if (tFoundit != sgpImplementation->mEvents.end())
        return;
    FMOD::Studio::EventDescription* pEventDescription = NULL;
    CAudioEngine::ErrorCheck(sgpImplementation->mpStudioSystem->getEvent(strEventName.c_str(), &pEventDescription));
    if (pEventDescription) {
        FMOD::Studio::EventInstance* pEventInstance = NULL;
        CAudioEngine::ErrorCheck(pEventDescription->createInstance(&pEventInstance));
        if (pEventInstance) {
            sgpImplementation->mEvents[strEventName] = pEventInstance;
        }
    }
}

void CAudioEngine::PlayEvent(const string& strEventName) {
    auto tFoundit = sgpImplementation->mEvents.find(strEventName);
    if (tFoundit == sgpImplementation->mEvents.end()) {
        LoadEvent(strEventName);
        tFoundit = sgpImplementation->mEvents.find(strEventName);
        if (tFoundit == sgpImplementation->mEvents.end())
            return;
    }
    tFoundit->second->start();
}

void CAudioEngine::StopEvent(const string& strEventName, bool bImmediate) {
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end())
        return;
    FMOD_STUDIO_STOP_MODE eMode;
    eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
    CAudioEngine::ErrorCheck(tFoundIt->second->stop(eMode));
}

bool CAudioEngine::IsEventPlaying(const string& strEventName) const {
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end())
        return false;

    FMOD_STUDIO_PLAYBACK_STATE state;
    CAudioEngine::ErrorCheck(tFoundIt->second->getPlaybackState(&state));
    return (state == FMOD_STUDIO_PLAYBACK_PLAYING);
}

void CAudioEngine::GetEventParameter(const string& strEventName, const string& strParameterName, float* parameter) {
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end())
        return;
    float value;
    // Use getParameterByName instead of getParameter
    CAudioEngine::ErrorCheck(tFoundIt->second->getParameterByName(strParameterName.c_str(), &value));
    *parameter = value;
}

void CAudioEngine::SetEventParameter(const string& strEventName, const string& strParameterName, float fValue) {
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end())
        return;
    // Use setParameterByName instead of getParameter and setValue
    CAudioEngine::ErrorCheck(tFoundIt->second->setParameterByName(strParameterName.c_str(), fValue));
}

FMOD_VECTOR CAudioEngine::VectorToFmod(const Vector3& vPosition) {
    FMOD_VECTOR fVec;
    fVec.x = vPosition.x;
    fVec.y = vPosition.y;
    fVec.z = vPosition.z;
    return fVec;
}

int CAudioEngine::ErrorCheck(FMOD_RESULT result) {
    if (result != FMOD_OK) {
        cout << "FMOD ERROR " << result << endl;
        return 1;
    }
    // cout << "FMOD all good" << endl;
    return 0;
}

float  CAudioEngine::dbToVolume(float dB)
{
    return powf(10.0f, 0.05f * dB);
}

float  CAudioEngine::VolumeTodB(float volume)
{
    return 20.0f * log10f(volume);
}

void CAudioEngine::Shutdown() {
    delete sgpImplementation;
}

void CAudioEngine::Set3dListenerAndOrientation(const Vector3& vPosition, const Vector3& vLook, const Vector3& vUp)
{
    // Convert our own vectors to FMOD vectors.
    FMOD_VECTOR pos = VectorToFmod(vPosition);
    // Here we assume zero velocity.
    FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
    FMOD_VECTOR forward = VectorToFmod(vLook);
    FMOD_VECTOR up = VectorToFmod(vUp);

    // Set listener attributes for listener index 0.
    CAudioEngine::ErrorCheck(sgpImplementation->mpSystem->set3DListenerAttributes(0, &pos, &vel, &forward, &up));
}

void CAudioEngine::StopChannel(int nChannelId)
{
    // Find the channel in the map using the channel id.
    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->mChannels.end())
    {
        // No such channel, nothing to stop.
        return;
    }

    // Stop the channel.
    CAudioEngine::ErrorCheck(tFoundIt->second->stop());
    // (The channel will be removed from mChannels later in Implementation::Update when isPlaying returns false.)
}

void CAudioEngine::StopAllChannels()
{
    // Iterate over all channels and stop each one.
    for (auto& channelPair : sgpImplementation->mChannels)
    {
        CAudioEngine::ErrorCheck(channelPair.second->stop());
    }
    // Clear the channel map.
    sgpImplementation->mChannels.clear();
}

bool CAudioEngine::IsPlaying(int nChannelId) const
{
    // Find the channel in our channel map.
    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->mChannels.end())
    {
        // If we don't have the channel, we assume it is not playing.
        return false;
    }

    bool bIsPlaying = false;
    // Query FMOD for the channel's playing status.
    CAudioEngine::ErrorCheck(tFoundIt->second->isPlaying(&bIsPlaying));
    return bIsPlaying;
}
