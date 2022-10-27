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
		VTX_DEBUG( "Starting gpu work: {}", Util::Math::to_string( _sizeComputed ) );
		_gl->glDispatchCompute( _sizeComputed.x, _sizeComputed.y, _sizeComputed.z );
		_gl->glMemoryBarrier( _barrier );

		if ( _force )
		{
			_gl->glFlush();
			_gl->glFinish();
		}
	}

	void GpuComputer::start( const Vec3i & p_size, const GLbitfield p_barrier )
	{
		_sizeComputed = p_size;
		setBarrier( p_barrier );
		start();
	}

	void GpuComputer::start( const uint p_taskCount, const GLbitfield p_barrier )
	{
		_sizeComputed = _computeSize( p_taskCount );
		setBarrier( p_barrier );
		start();
	}

	const Vec3i GpuComputer::_computeSize( const uint p_taskCount ) const
	{
		const int workGroupNeeded = static_cast<int>(
			std::ceil( ( p_taskCount % _size != 0 ) ? ( p_taskCount / static_cast<float>( _size + 1 ) )
													: ( p_taskCount / static_cast<float>( _size ) ) ) );

		const int xDimension = std::min( workGroupNeeded, VTX_SPEC().glMaxComputeWorkGroupCount[ 0 ] );
		const int yDimension = static_cast<int>( std::max(
			std::ceil( xDimension / static_cast<float>( float( VTX_SPEC().glMaxComputeWorkGroupCount[ 1 ] ) ) ) + 1,
			1.f ) );

		const int zDimension = 1;
		return { xDimension, yDimension, zDimension };
	}
} // namespace VTX::Worker
