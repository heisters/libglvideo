#pragma once
#include <vector>
#include <thread>
#include <atomic>
#include "concurrency.h"
#include "GLContext.h"
#include "FrameTexture.h"

namespace glvideo {
typedef std::function< void( GLContext::ref ) > context_job;

class Worker
{
public:
	typedef std::shared_ptr< Worker > ref;

	Worker( concurrent_queue< context_job > & queue );
	
	Worker( const Worker& ) = delete;
	Worker& operator=( const Worker& ) = delete;

	~Worker();

private:
	void work();

	GLContext::ref m_glContext = nullptr;
	concurrent_queue< context_job > & m_queue;
	std::thread m_thread;
	std::atomic_bool m_doWork{false};
};

class Context
{
public:
	typedef std::shared_ptr< Context > ref;

	static ref create( size_t nWorkers )
	{
		return std::make_shared< Context >( nWorkers );
	}

	Context( size_t nWorkers );

	Context( const Context& ) = delete;
	Context& operator=( const Context& ) = delete;

	void queueJob( context_job job );

private:
	std::vector< Worker::ref > m_workers;
	concurrent_queue< context_job > m_jobQueue;
};

}
