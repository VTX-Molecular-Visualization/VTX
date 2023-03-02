#ifndef __VTX_WORKER_GPU_BUFFER_INITIALIZER__
#define __VTX_WORKER_GPU_BUFFER_INITIALIZER__

#include "gpu_computer.hpp"
#include "renderer/gl/buffer.hpp"

namespace VTX::Worker
{
	class GpuBufferInitializer : public GpuComputer
	{
	  public:
		explicit GpuBufferInitializer( Renderer::GL::Buffer & p_buffer, const size_t p_size ) :
			GpuComputer( IO::FilePath( "ses/buffer_init.comp" ), p_size ), _buffer( p_buffer ), _bufferSize( p_size )
		{
		}

		void start() override
		{
			_buffer.bind( Renderer::GL::Buffer::Target::SHADER_STORAGE_BUFFER, 0 );
			GpuComputer::start();
			_buffer.unbind();
		}

	  private:
		const size_t		   _bufferSize;
		Renderer::GL::Buffer & _buffer;

		void _setUniforms() override { _program->setUInt( "uSize", uint( _bufferSize ) ); }
	};
} // namespace VTX::Worker

#endif
