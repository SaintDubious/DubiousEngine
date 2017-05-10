#include "Timer.h"

namespace Dubious {
namespace Utility {

Timer::Timer()
{
    start();
}

void Timer::start()
{
    m_start_time = std::chrono::steady_clock::now();
}

int64_t Timer::restart()
{
    int64_t milli = elapsed();
    start();
    return milli;
}

int64_t Timer::elapsed() const
{
    std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end_time - m_start_time).count();
}

}}
