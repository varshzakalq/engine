#pragma once
#include <chrono>

class Timer {
private:
    // Tracks the time point of the previous frame
    std::chrono::time_point<std::chrono::high_resolution_clock> m_last_time;

public:
    Timer() {
        // Initialize the timer starting right now
        m_last_time = std::chrono::high_resolution_clock::now();
    }

    // Calculates elapsed time since the last call in seconds
    float get_frame_time() {
        auto current_time = std::chrono::high_resolution_clock::now();
        
        // Compute duration between current frame and last frame in seconds
        std::chrono::duration<float> elapsed = current_time - m_last_time;
        
        // Update last_time to the current frame's time for the next tick
        m_last_time = current_time;
        
        // Cap delta time to prevent massive jumps during debugging or lag spikes
        if (elapsed.count() > 0.1f) {
            return 0.1f; 
        }

        return elapsed.count();
    }
};