#include "gpu_computer.hpp"
#include "vtx_app.hpp"

namespace VTX::Worker
{
	void GpuComputer::_run()
	{
		_program->use();

		_setUniforms();

		start();
	}

	void GpuComputer::start()
	{
		_gl->glDispatchCompute( _size.x, _size.y, _size.z );
		_gl->glMemoryBarrier( _barrier );

		if ( _force )
		{
			_gl->glFlush();
			_gl->glFinish();
		}
	}

	const Vec3i GpuComputer::_computeSize( const uint p_taskCount ) const
	{
		// ??
		Vec3i size = Vec3i( 64, 1, 1 );

		const uint workGroupNeeded
			= ( p_taskCount % size.x != 0 ) ? ( p_taskCount / size.x + 1 ) : ( p_taskCount / size.x );

		const uint xDimension = Util::Math::min( workGroupNeeded, uint( VTX_SPEC().glMaxComputeWorkGroupCount[ 0 ] ) );
		const uint yDimension
			= uint( Util::Math::max( workGroupNeeded / float( VTX_SPEC().glMaxComputeWorkGroupCount[ 0 ] ), 1.f ) );
		return Vec3i( xDimension, yDimension, 1 );
	}
} // namespace VTX::Worker
