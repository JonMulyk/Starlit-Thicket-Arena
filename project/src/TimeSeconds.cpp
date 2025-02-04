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