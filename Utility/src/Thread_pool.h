#ifndef INCLUDED_UTILITY_THREAD_POOL
#define INCLUDED_UTILITY_THREAD_POOL

#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>

namespace Dubious {
namespace Utility {

/// @brief Simple thread pool
///
/// This is a simple queue of work and a pool of threads to
/// do the work. It's not terribly sophisticated, and it
/// actually may not work correctly as it's not in use at
/// the moment. I am playing with threading and C++11 async, 
/// so this might go away.
///
/// DO NOT USE
/// Unless appropriately tested, debugged, etc

class Thread_pool {
public:
    Thread_pool( const int pool_size );
    ~Thread_pool();

    typedef std::function<void ()> Func;

    void                push( Func f );

    int                 size() const;

private:
    std::queue<Func>    m_queue;
    std::vector<std::thread> m_threads; 
    mutable std::mutex  m_mutex;
    std::condition_variable m_condition;
    bool                m_drain = false;

    void                thread_func();
};

}}

#endif

