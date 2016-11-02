#pragma once

// based on https://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html

#include <queue>
#include <mutex>
#include <condition_variable>

namespace glvideo {

template<typename Data>
class concurrent_queue {
protected:
    std::queue<Data> m_queue;
    mutable std::mutex m_mutex;
    std::condition_variable m_cv;

public:
    void push( Data const &data )
    {
        {
            std::lock_guard<std::mutex> lock( m_mutex );
            m_queue.push( data );
        }
        m_cv.notify_one();
    }

    void emplace( Data const &&data )
    {
        {
            std::lock_guard<std::mutex> lock( m_mutex );
            m_queue.emplace( data );
        }
        m_cv.notify_one();
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock( m_mutex );
        return m_queue.empty();
    }

    bool try_pop( Data *popped_value )
    {
        std::lock_guard<std::mutex> lock( m_mutex );
        if ( m_queue.empty()) {
            return false;
        }

        *popped_value = m_queue.front();
        m_queue.pop();
        return true;
    }

    void wait_and_pop( Data *popped_value )
    {
        std::unique_lock<std::mutex> lock( m_mutex );
        while ( m_queue.empty()) {
            m_cv.wait( lock );
        }

        *popped_value = m_queue.front();
        m_queue.pop();
    }
};


template<typename Data>
class concurrent_buffer : public concurrent_queue<Data> {
private:
    size_t m_maxSize;

public:
    concurrent_buffer() = delete;
    concurrent_buffer( size_t size ) : m_maxSize( size ) {}

    bool is_full() const
    {
        return remainingSize() == 0;
    }

	void clear()
	{
		std::lock_guard<std::mutex> lock( this->m_mutex );

		std::queue< Data > empty;
		std::swap( this->m_queue, empty );
	}

    size_t size() const { return m_maxSize; }
    size_t remainingSize() const
    {
        std::lock_guard<std::mutex> lock( this->m_mutex );
        return m_maxSize - this->m_queue.size();
    }
};

}