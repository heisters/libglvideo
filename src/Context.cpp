#include "Context.h"
#include "gl_load.h"

using namespace glvideo;
using namespace std;


Worker::Worker( concurrent_queue< context_job > & queue ) :
	m_queue( queue )
{
	m_doWork = true;

	m_thread = thread( bind( &Worker::work, this ) );
}

Worker::~Worker()
{
	m_doWork = false;
	if ( m_thread.joinable() ) m_thread.join();
}

void Worker::work()
{
	while ( m_doWork ) {
		context_job job;
		m_queue.wait_and_pop( &job );
		job();
	}
}




Context::Context( size_t nWorkers )
{
    assignGlFunctionPointers();

	for ( size_t i = 0; i < nWorkers; ++i ) {
		m_workers.push_back( make_shared< Worker >( m_jobQueue ) );
	}
}

void Context::queueJob( context_job job )
{
	m_jobQueue.push( job );
}

