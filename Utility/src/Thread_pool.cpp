#include "Thread_pool.h"

#include <iostream>

namespace Dubious {
namespace Utility {

Thread_pool::Thread_pool( const int pool_size )
{
    for (int i=0; i<pool_size; ++i) {
        m_threads.push_back( std::thread( &Thread_pool::thread_func, this ) );
    }
}

Thread_pool::~Thread_pool()
{
    try {
        {
            std::unique_lock<std::mutex> lock( m_mutex );
            while (!m_queue.empty()) {
                m_queue.pop();
            }
            m_drain = true;
        }
        m_condition.notify_all();
        for (auto& t : m_threads) {
            t.join();
        }
    }
    catch (...) {
    }
}

void Thread_pool::push( Func f )
{
    {
        std::unique_lock<std::mutex> lock( m_mutex );
        m_queue.push( f );
    }
    m_condition.notify_all();
}

int Thread_pool::size() const 
{
    std::unique_lock<std::mutex> lock( m_mutex );
    return m_queue.size();
}

void Thread_pool::thread_func()
{
    while (!m_drain) {
        std::unique_lock<std::mutex> lock( m_mutex );
        if (m_queue.empty()) {
            if (m_drain) {
                continue;
            }
            m_condition.wait( lock, [this](){ return m_drain || !m_queue.empty(); } );
            if (m_drain) {
                continue;
            }
        }
        Func f = m_queue.front();
        m_queue.pop();
        lock.unlock();
        f();
    }
}

}}
