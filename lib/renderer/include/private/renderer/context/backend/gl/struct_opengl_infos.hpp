#include <array>
#include <util/logger.hpp>
#include <util/string.hpp>

namespace VTX::Renderer::Context::GL
{
	enum E_GL_EXTENSIONS
	{
		NVX_gpu_memory_info = 0,
		ATI_meminfo,
		EXTENSIONS_COUNT
	};

	struct StructOpenglInfos
	{
		std::string glVendor;
		std::string glRenderer;
		std::string glVersion;
		std::string glslVersion;

		int glMaxUniformBlockSize;
		int glMaxUniformBufferBindings;
		int glMaxShaderStorageBlockSize;
		int glMaxShaderStorageBufferBindings;
		int glMaxTextureSize;
		int glMaxPatchVertices;
		int glMaxTessGenLevel;
		int glMaxComputeWorkGroupCount[ 3 ];
		int glMaxComputeWorkGroupSize[ 3 ];
		int glMaxComputeWorkGroupInvocations;

		std::array<bool, E_GL_EXTENSIONS::EXTENSIONS_COUNT> glExtensions = { false };

		void print()
		{
			VTX_INFO( "OpenGL infos:" );
			VTX_INFO( "Device: {} {}", glVendor, glRenderer );
			VTX_INFO( "Version: {}", glVersion );

			VTX_TRACE( "GLSL version: {}", glslVersion );
			VTX_TRACE( "Max uniform block size: {}", Util::String::memSizeToStr( glMaxUniformBlockSize, false ) );
			VTX_TRACE( "Max uniform buffer bindings: {}", glMaxUniformBufferBindings );
			VTX_TRACE( "Max shader storage block size: {}", Util::String::memSizeToStr( glMaxShaderStorageBlockSize ) );
			VTX_TRACE( "Max shader storage buffer bindings: {}", glMaxShaderStorageBufferBindings );
			VTX_TRACE( "Max texture size: {}", glMaxTextureSize );
			VTX_TRACE( "Max patch vertices: {}", glMaxPatchVertices );
			VTX_TRACE( "Max tess gen level: {}", glMaxTessGenLevel );
			VTX_TRACE(
				"Max compute work group count: {} {} {}",
				glMaxComputeWorkGroupCount[ 0 ],
				glMaxComputeWorkGroupCount[ 1 ],
				glMaxComputeWorkGroupCount[ 2 ]
			);
			VTX_TRACE(
				"Max compute work group size: {} {} {}",
				glMaxComputeWorkGroupSize[ 0 ],
				glMaxComputeWorkGroupSize[ 1 ],
				glMaxComputeWorkGroupSize[ 2 ]
			);
			VTX_TRACE( "Max compute work group invocations: {}", glMaxComputeWorkGroupInvocations );
		}
	};
} // namespace VTX::Renderer::Context::GL
