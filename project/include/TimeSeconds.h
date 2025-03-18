#pragma once
#include <GLFW/glfw3.h>
#include <string>

class TimeSeconds {
private:
    double m_newTime = 0.0;
    double m_currentTime;
    double m_frameTime = 0.0;
    double m_accumulator = 0.0;
    double m_elapsedTime = 0.0;

public:
    // Fixed time step (seconds per update)
    const double dt = 1.0 / 60.0;

    TimeSeconds();

    // Call once per frame to update timing values.
    void tick();

    // Call repeatedly until the accumulator is less than the time step.
    void advance();

    // helpers
    double getFPS() const;

    // Getters
    double getCurrentTime() const;
    double getFrameTime() const;
    double getAccumultor() const;
    double getElapsedTime() const;
    double getRemainingTime(double countDownDuration) const;

    std::string formatTimeToHumanReadable(double timeInSeconds) const;

    void reset();
};
