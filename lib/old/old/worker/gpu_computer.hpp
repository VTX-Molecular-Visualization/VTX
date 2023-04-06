#ifndef __VTX_WORKER_GPU_COMPUTER__
#define __VTX_WORKER_GPU_COMPUTER__

#include "base_worker.hpp"
#include "generic/base_opengl.hpp"
#include "renderer/gl/program.hpp"
#include "renderer/gl/program_manager.hpp"
#include <util/types.hpp>
#include <vector>

#define LOCAL_SIZE_X 256
#define LOCAL_SIZE_Y 1
#define LOCAL_SIZE_Z 1

namespace VTX::Worker
{
	class GpuComputer : public Worker::BaseWorker, public Generic::BaseOpenGL
	{
	  public:
		explicit GpuComputer( const FilePath &										   p_shader,
							  const size_t											   p_size,
							  const std::vector<std::pair<std::string, std::string>> & p_customDefines = {} ) :
			GpuComputer( p_shader, _computeSize( p_size ), p_customDefines )
		{
		}

		explicit GpuComputer( const FilePath & p_shader,
							  const Vec3i &	   p_size = Vec3i( LOCAL_SIZE_X, LOCAL_SIZE_Y, LOCAL_SIZE_Z ),
							  const std::vector<std::pair<std::string, std::string>> & p_customDefines = {} ) :
			_size( p_size )

		{
			std::string definesToInject = "#define LOCAL_SIZE_X " + std::to_string( LOCAL_SIZE_X ) + "\n"
										  + "#define LOCAL_SIZE_Y " + std::to_string( LOCAL_SIZE_Y ) + "\n"
										  + "#define LOCAL_SIZE_Z " + std::to_string( LOCAL_SIZE_Z ) + "\n";

			std::string shaderNameSuffix = "";
			for ( const std::pair<std::string, std::string> & p_define : p_customDefines )
			{
				definesToInject += "#define " + p_define.first + " " + p_define.second + "\n";
				shaderNameSuffix += p_define.second;
			}

			_program = VTX_PROGRAM_MANAGER().createProgram(
				p_shader.stem().string(), { p_shader }, definesToInject, shaderNameSuffix );
		}

		virtual ~GpuComputer() = default;

		inline Renderer::GL::Program & getProgram() { return *_program; }

		virtual void start();
		void		 start( const Vec3i & );
		void		 start( const size_t );

	  protected:
		Renderer::GL::Program * _program;
		Vec3i					_size;

		virtual void _run() override;

	  private:
		virtual void _setUniforms() {}
		const Vec3i	 _computeSize( const size_t ) const;
	};
} // namespace VTX::Worker

#endif
