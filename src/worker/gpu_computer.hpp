#ifndef __VTX_WORKER_GPU_COMPUTER__
#define __VTX_WORKER_GPU_COMPUTER__

#include "base_worker.hpp"
#include "define.hpp"
#include "generic/base_opengl.hpp"
#include "renderer/gl/program.hpp"
#include "renderer/gl/program_manager.hpp"
#include <vector>

namespace VTX::Worker
{
	class GpuComputer : public Worker::BaseWorker, public Generic::BaseOpenGL
	{
	  public:
		explicit GpuComputer( const IO::FilePath & p_shader,
							  const Vec3i &		   p_size,
							  const GLbitfield	   p_barrier = 0,
							  const bool		   p_force	 = false ) :
			_size( p_size ),
			_barrier( p_barrier ), _force( p_force ),
			_program( VTX_PROGRAM_MANAGER().createProgram( p_shader.filenameWithoutExtension(), { p_shader } ) )
		{
		}
		explicit GpuComputer( const IO::FilePath & p_shader,
							  const uint		   p_taskCount,
							  const GLbitfield	   p_barrier = 0,
							  const bool		   p_force	 = false ) :
			GpuComputer( p_shader, _computeSize( p_taskCount ), p_barrier, p_force )
		{
		}

		virtual ~GpuComputer() = default;

		inline Renderer::GL::Program & getProgram() { return *_program; }
		inline void					   setSize( const Vec3i & p_size ) { _size = p_size; }
		inline void					   setTaskCount( const uint p_taskCount ) { _size = _computeSize( p_taskCount ); }
		inline void					   setBarrier( const GLbitfield p_barrier ) { _barrier = p_barrier; }
		inline void					   setForce( const bool p_force ) { _force = p_force; }

		void start();

	  protected:
		Renderer::GL::Program * const _program;
		Vec3i						  _size;
		GLbitfield					  _barrier;
		bool						  _force;

		virtual void _run() override;

	  private:
		virtual void _setUniforms() {}
		const Vec3i	 _computeSize( const uint ) const;
	};
} // namespace VTX::Worker

#endif
