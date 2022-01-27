#ifndef __VTX_WORKER_GPU_WORKER__
#define __VTX_WORKER_GPU_WORKER__

#include "base_worker.hpp"
#include "define.hpp"
#include "generic/base_opengl.hpp"
#include "renderer/gl/program.hpp"
#include "renderer/gl/program_manager.hpp"
#include <vector>

namespace VTX::Worker
{
	class BaseGpuWorker : public Worker::BaseWorker, public Generic::BaseOpenGL
	{
	  public:
		explicit BaseGpuWorker( const IO::FilePath & p_shader,
								const Vec3i &		 p_size,
								const GLbitfield	 p_barrier = 0,
								const bool			 p_force   = true ) :
			_size( p_size ),
			_barrier( p_barrier ), _force( p_force )
		{
			_program = VTX_PROGRAM_MANAGER().createProgram( p_shader.filenameWithoutExtension(), { p_shader } );
		}
		virtual ~BaseGpuWorker() = default;

	  protected:
		Renderer::GL::Program * _program;
		Vec3i					_size;
		const GLbitfield		_barrier = 0;
		const bool				_force;

		virtual void _run() override;

	  private:
		virtual void _setUniforms() = 0;
	};
} // namespace VTX::Worker

#endif
