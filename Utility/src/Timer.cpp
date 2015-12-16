#include "Timer.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Utility {

//////////////////////////////////////////////////////////////
Timer::Timer()
{
    Start();
}

//////////////////////////////////////////////////////////////
void Timer::Start()
{
    m_StartTime = std::chrono::steady_clock::now();
}

//////////////////////////////////////////////////////////////
int64_t Timer::Restart()
{
    int64_t Milli = Elapsed();
    Start();
    return Milli;
}

//////////////////////////////////////////////////////////////
int64_t Timer::Elapsed() const
{
    std::chrono::steady_clock::time_point EndTime = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(EndTime - m_StartTime).count();
}

}}
