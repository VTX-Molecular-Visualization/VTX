#include "app/render/worker/gpu_computer.hpp"
#include "app/old_app/vtx_app.hpp"

namespace VTX::App::Render::Worker
{
	void GpuComputer::_run()
	{
		_program->use();
		_setUniforms();
		start();
	}

	// TODO reimplement this without Qt (OpenGl embedded with Qt)
	void GpuComputer::start()
	{
		//// VTX_DEBUG( "Starting gpu work: {}", Util::to_string_fmt( _size ) );

		//_gl->glDispatchCompute( _size.x, _size.y, _size.z );
	}

	void GpuComputer::start( const Vec3i & p_size )
	{
		_size = p_size;

		start();
	}

	void GpuComputer::start( const size_t p_taskCount )
	{
		_size = _computeSize( p_taskCount );

		start();
	}

	const Vec3i GpuComputer::_computeSize( const size_t p_taskCount ) const
	{
		// VTX_DEBUG( "Gpu work compute task count: {} and local size: {}", p_taskCount, LOCAL_SIZE_X );

		const int workGroupNeeded = int( ( float( p_taskCount ) / LOCAL_SIZE_X ) + 1.f );

		// TODO: distribute work groups on multiple dimensions.
		assert( workGroupNeeded <= VTX_SPEC().glMaxComputeWorkGroupCount[ 0 ] );

		// VTX_DEBUG( "Gpu work compute work group needed: {}", workGroupNeeded );

		const int xDimension = std::min( workGroupNeeded, VTX_SPEC().glMaxComputeWorkGroupCount[ 0 ] );
		const int yDimension = 1;
		const int zDimension = 1;

		return { xDimension, yDimension, zDimension };
	}
} // namespace VTX::App::Render::Worker
