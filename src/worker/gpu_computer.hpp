#ifndef __VTX_WORKER_GPU_COMPUTER__
#define __VTX_WORKER_GPU_COMPUTER__

#include "base_worker.hpp"
#include "define.hpp"
#include "generic/base_opengl.hpp"
#include "renderer/gl/program.hpp"
#include "renderer/gl/program_manager.hpp"
#include <vector>

#define LOCAL_SIZE_X 256

namespace VTX::Worker
{
	class GpuComputer : public Worker::BaseWorker, public Generic::BaseOpenGL
	{
	  public:
		explicit GpuComputer( const IO::FilePath & p_shader,
							  const Vec3i &		   p_size	 = Vec3i( LOCAL_SIZE_X, 1, 1 ),
							  const GLbitfield	   p_barrier = 0,
							  const bool		   p_force	 = false ) :
			_size( p_size ),
			_barrier( p_barrier ), _force( p_force ),
			_program( VTX_PROGRAM_MANAGER().createProgram( p_shader.filenameWithoutExtension(), { p_shader } ) )
		{
		}

		virtual ~GpuComputer() = default;

		inline Renderer::GL::Program & getProgram() { return *_program; }
		inline void					   setBarrier( const GLbitfield p_barrier ) { _barrier = p_barrier; }
		inline void					   setForce( const bool p_force ) { _force = p_force; }

		void start();
		void start( const Vec3i &, const GLbitfield = 0 );
		void start( const uint, const GLbitfield = 0 );

	  protected:
		Renderer::GL::Program * const _program;
		Vec3i						  _size;
		// uint						  _size;
		GLbitfield _barrier;
		bool	   _force;

		virtual void _run() override;

	  private:
		virtual void _setUniforms() {}
		const Vec3i	 _computeSize( const uint ) const;
	};
} // namespace VTX::Worker

#endif
