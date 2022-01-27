#include "base_gpu_worker.hpp"

namespace VTX::Worker
{
	void BaseGpuWorker::_run()
	{
		_program->use();

		_setUniforms();

		_gl->glDispatchCompute( _size.x, _size.y, _size.z );
		_gl->glMemoryBarrier( _barrier );

		if ( _force )
		{
			_gl->glFlush();
			_gl->glFinish();
		}
	}
} // namespace VTX::Worker
