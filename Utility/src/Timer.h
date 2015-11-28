#ifndef INCLUDED_UTILITY_TIMER
#define INCLUDED_UTILITY_TIMER

#include <chrono>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Utility {

/// @brief A Timer
///
/// The Timer class is a simple thing used to keep track of how much
///	time has passed. 
class Timer
{
public:
    /// @brief Constructor. Calls Start
    Timer();

    Timer( const Timer& ) = delete;

    /// @brief Destructor
    ~Timer() = default;

    Timer& operator=( const Timer& ) = delete;

    /// @brief Starts the timer
    void                Start();

    /// @brief Timer Restart
    ///
    /// Restarts the timer and returns how much time has elapsed
    ///	since the last call to Start or Restart
    /// @returns milliseconds since last Start or Restart
    int64_t             Restart();

    /// @brief Elapsed Time
    ///
    /// Gets the number of milliseconds since the last call to
    ///	Start or Restart.  Does not restart the timer.
    /// @returns elapsed milliseconds since last Start or Restart
    int64_t             Elapsed() const;

private:
    std::chrono::steady_clock::time_point m_StartTime;
};

}
}

#endif
