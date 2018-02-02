#include "Context.h"
#include "gl_includes.h"

using namespace glvideo;
using namespace std;

#ifndef GL_VERSION_3_2
#error OpenGL version 3.2 is not supported
#endif


Worker::Worker( concurrent_queue< context_job > & queue ) :
	m_queue( queue )
{
	m_doWork = true;

	m_thread = thread( bind( &Worker::work, this ) );
}

Worker::~Worker()
{
	m_doWork = false;
    m_queue.interrupt();
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
    if ( ! gladLoadGL() ) {
        throw std::runtime_error( "Could not load OpenGL in libglvideo" );
    }

    if ( ! GLAD_GL_VERSION_3_2 ) {
        throw std::runtime_error( "OpenGL version 3.2 not loaded" );
    }

	for ( size_t i = 0; i < nWorkers; ++i ) {
		m_workers.push_back( make_shared< Worker >( m_jobQueue ) );
	}
}

void Context::queueJob( context_job job )
{
	m_jobQueue.push( job );
}

