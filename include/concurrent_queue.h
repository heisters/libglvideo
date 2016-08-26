#pragma once

// based on https://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html

#include <queue>
#include <mutex>
#include <condition_variable>

namespace glvideo {

template<typename Data>
class concurrent_queue {
private:
    std::queue<Data> mQueue;
    mutable std::mutex mMutex;
    std::condition_variable mCV;

public:
    void push( Data const &data )
    {
        {
            std::lock_guard<std::mutex> lock( mMutex );
            mQueue.push( data );
        }
        mCV.notify_one();
    }

    void emplace( Data const &&data )
    {
        {
            std::lock_guard<std::mutex> lock( mMutex );
            mQueue.emplace( data );
        }
        mCV.notify_one();
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock( mMutex );
        return mQueue.empty();
    }

    bool try_pop( Data *popped_value )
    {
        std::lock_guard<std::mutex> lock( mMutex );
        if ( mQueue.empty()) {
            return false;
        }

        *popped_value = mQueue.front();
        mQueue.pop();
        return true;
    }

    void wait_and_pop( Data *popped_value )
    {
        std::lock_guard<std::mutex> lock( mMutex );
        while ( mQueue.empty()) {
            mCV.wait( lock );
        }

        *popped_value = mQueue.front();
        mQueue.pop();
    }
};

}