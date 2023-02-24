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
		VTX_DEBUG( "Starting gpu work: {}", Util::Math::to_string( _size ) );

		if ( _barrierPre )
		{
			_gl->glMemoryBarrier( _barrierPre );
		}

		_gl->glDispatchCompute( _size.x, _size.y, _size.z );

		if ( _barrierPost )
		{
			_gl->glMemoryBarrier( _barrierPost );
		}

		if ( _force )
		{
			_gl->glFlush();
			//_gl->glFinish();
		}
	}

	void GpuComputer::start( const Vec3i & p_size, const GLbitfield p_barrier )
	{
		_size = p_size;
		setBarrierPost( p_barrier );
		start();
	}

	void GpuComputer::start( const size_t p_taskCount, const GLbitfield p_barrier )
	{
		_size = _computeSize( p_taskCount );
		setBarrierPost( p_barrier );
		start();
	}

	const Vec3i GpuComputer::_computeSize( const size_t p_taskCount ) const
	{
		VTX_DEBUG( "Gpu work compute task count: {} and local size: {}", p_taskCount, LOCAL_SIZE_X );

		const int workGroupNeeded = int( ( float( p_taskCount ) / LOCAL_SIZE_X ) + 1.f );

		// TODO: distribute work groups on multiple dimensions.
		assert( workGroupNeeded <= VTX_SPEC().glMaxComputeWorkGroupCount[ 0 ] );

		VTX_DEBUG( "Gpu work compute work group needed: {}", workGroupNeeded );

		const int xDimension = std::min( workGroupNeeded, VTX_SPEC().glMaxComputeWorkGroupCount[ 0 ] );
		const int yDimension = 1;
		const int zDimension = 1;

		return { xDimension, yDimension, zDimension };
	}
} // namespace VTX::Worker
