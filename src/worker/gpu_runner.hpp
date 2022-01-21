#ifndef __VTX_WORKER_GPU_RUNNER__
#define __VTX_WORKER_GPU_RUNNER__

#include "base_worker.hpp"
#include "define.hpp"
#include "generic/base_opengl.hpp"
#include "renderer/gl/program.hpp"
#include <vector>

namespace VTX::Worker
{
	class GpuRunner : public Worker::BaseWorker, public Generic::BaseOpenGL
	{
	  public:
		explicit GpuRunner( Renderer::GL::Program * const p_program,
							const Vec3i &				  p_size,
							const GLbitfield			  p_barrier,
							const bool					  p_force ) :
			_program( p_program ),
			_size( p_size ), _barrier( p_barrier ), _force( p_force )
		{
		}

	  protected:
		void _run() override;

	  private:
		Renderer::GL::Program * _program;
		const Vec3i				_size;
		const GLbitfield		_barrier = 0;
		const bool				_force;
	};
} // namespace VTX::Worker

#endif
