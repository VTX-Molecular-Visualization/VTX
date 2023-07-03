#ifndef __VTX_APP_OLD_RENDDER_SPEC__
#define __VTX_APP_OLD_RENDDER_SPEC__

#include <string>
#include <util/logger.hpp>

namespace VTX::App::Old::Render
{
	struct Spec
	{
		std::string glVendor;
		std::string glRenderer;
		std::string glVersion;
		std::string glslVersion;
		std::string glExtensions;

		int glMaxTextureSize;
		int glMaxPatchVertices;
		int glMaxTessGenLevel;
		int glMaxComputeWorkGroupCount[ 3 ];
		int glMaxComputeWorkGroupSize[ 3 ];
		int glMaxComputeWorkGroupInvocations;
		int glMaxUniformBlockSize;
		int glMaxShaderStorageBlockSize;
		int glMaxShaderStorageBufferBindings;

		void print()
		{
			VTX_INFO( "GL device: {} {}", glVendor, glRenderer );
			VTX_INFO( "GL version: {}", glVersion );
			VTX_INFO( "GLSL version: {}", glslVersion );
			// VTX_INFO( "GL extensions: " + glExtensions );

			VTX_DEBUG( "Max texture size : {}", glMaxTextureSize );
			VTX_DEBUG( "Max patch vertices: {}", glMaxPatchVertices );
			VTX_DEBUG( "Max tessellation gen level: {}", glMaxTessGenLevel );

			VTX_DEBUG( "Max compute work group count: {} {} {}",
					   glMaxComputeWorkGroupCount[ 0 ],
					   glMaxComputeWorkGroupCount[ 1 ],
					   glMaxComputeWorkGroupCount[ 2 ] );
			VTX_DEBUG( "Max compute work group size: {} {} {}",
					   glMaxComputeWorkGroupSize[ 0 ],
					   glMaxComputeWorkGroupSize[ 1 ],
					   glMaxComputeWorkGroupSize[ 2 ] );
			VTX_DEBUG( "Max compute work group invocations: {}", glMaxComputeWorkGroupInvocations );
			VTX_DEBUG( "Max uniform block size: {}", glMaxUniformBlockSize );
			VTX_DEBUG( "Max shader storage block size: {}", glMaxShaderStorageBlockSize );
			VTX_DEBUG( "Max shader storage buffer bindings: {}", glMaxShaderStorageBufferBindings );
		}
	};
} // namespace VTX::App::Old::Render
#endif
