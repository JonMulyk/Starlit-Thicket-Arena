#include "TimeSeconds.h"
#include <iomanip>
#include <sstream>

// Ctor
TimeSeconds::TimeSeconds() : m_currentTime(glfwGetTime()), m_stopped(false) {}

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


std::string TimeSeconds::formatTimeToHumanReadable(double timeInSeconds) const
{
    int minutes = timeInSeconds / 60;
    int seconds = static_cast<int>(timeInSeconds) % 60;
    
    std::ostringstream stream;
    stream  << std::setw(2) << minutes << ":" 
            << std::setw(2) << std::setfill('0') << seconds;
    return stream.str();

    // change formatting on this
}

void TimeSeconds::reset() {
    m_currentTime = glfwGetTime();  // Reset to current time
    m_newTime = m_currentTime;
    m_accumulator = 0.0;
    m_elapsedTime = 0.0;  // Critical: ensure elapsed time starts at 0
}

// Stop time progression
void TimeSeconds::stop() {
    m_stopped = true;
}

// Resume time progression
void TimeSeconds::resume() {
    if (!m_stopped) return;

    m_stopped = false;
    m_currentTime = glfwGetTime(); // Reset reference point
}
