#ifndef INCLUDED_UTILITY_TIMER
#define INCLUDED_UTILITY_TIMER

#include <chrono>

namespace Dubious {
namespace Utility {

/// @brief A Timer
///
/// The Timer class is a simple thing used to keep track of how much
///	time has passed.
class Timer {
public:
    /// @brief Constructor. Calls Start
    Timer();

    Timer(const Timer&) = delete;

    Timer& operator=(const Timer&) = delete;

    /// @brief Starts the timer
    void start();

    /// @brief Timer Restart
    ///
    /// Restarts the timer and returns how much time has elapsed
    ///	since the last call to Start or Restart
    /// @returns milliseconds since last start or restart
    int64_t restart();

    /// @brief Elapsed Time
    ///
    /// Gets the number of milliseconds since the last call to
    ///	start or restart.  Does not restart the timer.
    /// @returns elapsed milliseconds since last start or restart
    int64_t elapsed() const;

private:
    std::chrono::steady_clock::time_point m_start_time;
};

}  // namespace Utility
}  // namespace Dubious

#endif
