#ifndef __VTX_WORKER_GPU_BUFFER_INITIALIZER__
#define __VTX_WORKER_GPU_BUFFER_INITIALIZER__

#include "gpu_computer.hpp"
#include "app/old_app/renderer/gl/buffer.hpp"

namespace VTX::Worker
{
	class GpuBufferInitializer : public GpuComputer
	{
	  public:
		explicit GpuBufferInitializer( Renderer::GL::Buffer & p_buffer,
									   const size_t			  p_size,
									   const std::string &	  p_type   = "uint",
									   const std::string &	  p_layout = "std430",
									   const std::string &	  p_value  = "0" ) :
			GpuComputer( FilePath( "ses/buffer_init.comp" ),
						 p_size,
						 { { "TYPE", p_type }, { "LAYOUT", p_layout }, { "VALUE", p_value } } ),
			_buffer( p_buffer ), _bufferSize( p_size ), _type( p_type ), _layout( p_layout ), _value( p_value )
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
		const std::string	   _type;
		const std::string	   _layout;
		const std::string	   _value;

		void _setUniforms() override { _program->setUInt( "uSize", uint( _bufferSize ) ); }
	};
} // namespace VTX::Worker

#endif
