#include "TimeSeconds.h"

// Ctor
TimeSeconds::TimeSeconds() : m_currentTime(glfwGetTime()) {}

void TimeSeconds::tick() {
    // Get the new time.
    m_newTime = glfwGetTime();

    // Update values.
    m_frameTime = m_newTime - m_currentTime;
    m_accumulator += m_frameTime;
    m_currentTime = m_newTime;
}

void TimeSeconds::advance() {
    m_accumulator -= dt;
    m_elapsedTime += dt;
}

double TimeSeconds::getFPS() const {
    return 1.0 / m_frameTime;
}

double TimeSeconds::getCurrentTime() const {
    return m_currentTime;
}

double TimeSeconds::getFrameTime() const {
    return m_frameTime;
}

double TimeSeconds::getAccumultor() const {
    return m_accumulator;
}

double TimeSeconds::getElapsedTime() const {
    return m_elapsedTime;
}

double TimeSeconds::getRemainingTime(double countDownDuration) const
{
    double timeLeft = countDownDuration - m_elapsedTime;
    if (timeLeft <= 0)
        return 0;

    return timeLeft;
}


std::string TimeSeconds::formatToHumanReadable(double timeInSeconds) const
{
    int minutes = timeInSeconds / 60;
    int seconds = static_cast<int>(timeInSeconds) % 60;

    return (std::to_string(minutes) + ":" + std::to_string(seconds));
}
