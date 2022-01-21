#include "gpu_runner.hpp"

namespace VTX::Worker
{
	void GpuRunner::_run()
	{
		_program->use();
		// TODO: uniforms!

		_gl->glDispatchCompute( _size.x, _size.y, _size.z );
		_gl->glMemoryBarrier( _barrier );

		if ( _force )
		{
			_gl->glFlush();
			_gl->glFinish();
		}
	}
} // namespace VTX::Worker
