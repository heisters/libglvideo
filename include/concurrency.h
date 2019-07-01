#pragma once

// based on https://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace glvideo {

template<typename Data>
class concurrent_queue {
public:
    typedef std::mutex mutex;

protected:
    std::queue< Data > m_queue;
    mutable mutex m_mutex;
    std::condition_variable m_cv;
    bool m_run = true;

public:
    virtual void abort()
    {
        {
            std::unique_lock< mutex > lock( m_mutex );
            m_run = false;
            m_cv.notify_all();
        }
    }

    virtual void push( Data const &data )
    {
        bool wasEmpty = false;
        {
            std::unique_lock< mutex > lock( m_mutex );
            wasEmpty = m_queue.empty();
            m_queue.push( data );
        }
        if ( wasEmpty ) m_cv.notify_one();
    }

    virtual void emplace( Data const &&data )
    {
        bool wasEmpty = false;
        {
            std::unique_lock< mutex > lock( m_mutex );
            wasEmpty = m_queue.empty();
            m_queue.emplace( data );
        }
        if ( wasEmpty ) m_cv.notify_one();
    }

    bool empty() const
    {
        std::unique_lock< mutex > lock( m_mutex );
        return m_queue.empty();
    }

    bool try_pop( Data *popped_value )
    {
        std::unique_lock< mutex > lock( m_mutex );
        if ( m_queue.empty()) {
            return false;
        }

        *popped_value = m_queue.front();
        m_queue.pop();
        return true;
    }

    void wait_and_pop( Data *popped_value )
    {
        std::unique_lock< mutex > lock( m_mutex );
        m_cv.wait( lock, [this]() -> bool { return ! m_queue.empty() || ! m_run; } );

        if ( ! m_run || m_queue.empty() ) return;

        *popped_value = m_queue.front();
        m_queue.pop();
    }
};


template<typename Data>
class concurrent_buffer : public concurrent_queue<Data> {
private:
    size_t m_maxSize;

public:
    typedef std::mutex mutex;

    class buffer_full_error : public std::logic_error {
    public:
        explicit buffer_full_error( std::string what = "attempting to add element to a full buffer" ) :
            std::logic_error( what ) { }
    };

    concurrent_buffer() = delete;
    concurrent_buffer( size_t size ) : m_maxSize( size ) {}

    virtual void push( Data const & data )
    {
        if ( is_full() ) throw buffer_full_error();
        concurrent_queue< Data >::push( data );
    }

    virtual void emplace( Data const && data )
    {
        if ( is_full() ) throw buffer_full_error();
        concurrent_queue< Data >::emplace( std::move( data ) );
    }

    bool try_push( Data const & data )
    {
        bool success = true;
        {
            std::lock_guard< mutex > lock( this->m_mutex );
            if ( this->m_queue.size() < m_maxSize ) {
                this->m_queue.push( data );
                success = true;
            }
        }
        this->m_cv.notify_one();
        return success;
    }

    bool try_emplace( Data const && data )
    {
        bool success = true;
        {
            std::lock_guard< mutex > lock( this->m_mutex );
            if ( this->m_queue.size() < m_maxSize ) {
                this->m_queue.emplace( data );
                success = true;
            }
        }
        this->m_cv.notify_one();
        return success;
    }

    bool is_full() const
    {
        return remainingSize() == 0;
    }

	void clear()
	{
		std::lock_guard< mutex > lock( this->m_mutex );

		std::queue< Data > empty;
		std::swap( this->m_queue, empty );
	}

    size_t size() const { return m_maxSize; }
    size_t remainingSize() const
    {
        std::lock_guard< mutex > lock( this->m_mutex );
        return std::max( 0, (int)m_maxSize - (int)this->m_queue.size() );
    }
};

}
