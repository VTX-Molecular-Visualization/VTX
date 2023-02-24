#ifndef __VTX_WORKER_GPU_COMPUTER__
#define __VTX_WORKER_GPU_COMPUTER__

#include "base_worker.hpp"
#include "define.hpp"
#include "generic/base_opengl.hpp"
#include "renderer/gl/program.hpp"
#include "renderer/gl/program_manager.hpp"
#include <vector>

#define LOCAL_SIZE_X 256
#define LOCAL_SIZE_Y 1
#define LOCAL_SIZE_Z 1

namespace VTX::Worker
{
	class GpuComputer : public Worker::BaseWorker, public Generic::BaseOpenGL
	{
	  public:
		explicit GpuComputer( const IO::FilePath & p_shader,
							  const Vec3i &		   p_size	 = Vec3i( LOCAL_SIZE_X, LOCAL_SIZE_Y, LOCAL_SIZE_Z ),
							  const GLbitfield	   p_barrier = 0,
							  const bool		   p_force	 = false ) :
			_size( p_size ),
			_barrier( p_barrier ), _force( p_force )
		{
			const std::string definesToInject = "#define LOCAL_SIZE_X " + std::to_string( LOCAL_SIZE_X ) + "\n"
												+ "#define LOCAL_SIZE_Y " + std::to_string( LOCAL_SIZE_Y ) + "\n"
												+ "#define LOCAL_SIZE_Z " + std::to_string( LOCAL_SIZE_Z ) + "\n";
			_program = VTX_PROGRAM_MANAGER().createProgram(
				p_shader.filenameWithoutExtension(), { p_shader }, definesToInject );
		}

		virtual ~GpuComputer() = default;

		inline Renderer::GL::Program & getProgram() { return *_program; }
		inline void					   setBarrier( const GLbitfield p_barrier ) { _barrier = p_barrier; }
		inline void					   setForce( const bool p_force ) { _force = p_force; }

		void start();
		void start( const Vec3i &, const GLbitfield = 0 );
		void start( const size_t, const GLbitfield = 0 );

	  protected:
		Renderer::GL::Program * _program;
		Vec3i					_size;
		GLbitfield				_barrier = 0;
		bool					_force;

		virtual void _run() override;

	  private:
		virtual void _setUniforms() {}
		const Vec3i	 _computeSize( const size_t ) const;
	};
} // namespace VTX::Worker

#endif
