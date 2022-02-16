#ifndef __VTX_SPEC__
#define __VTX_SPEC__

#include "tool/logger.hpp"

namespace VTX
{
	struct Spec
	{
		std::string glVendor;
		std::string glRenderer;
		std::string glVersion;
		std::string glslVersion;

		int glMaxTextureSize;
		int glMaxPatchVertices;
		int glMaxTessGenLevel;
		int glMaxComputeWorkGroupCount[ 3 ];
		int glMaxComputeWorkGroupSize[ 3 ];
		int glMaxComputeWorkGroupInvocations;

		void print()
		{
			VTX_INFO( "GL device: " + glVendor + " " + glRenderer );
			VTX_INFO( "GL version: " + glVersion );
			VTX_INFO( "GLSL version: " + glslVersion );

			VTX_DEBUG( "Max texture size : " + std::to_string( glMaxTextureSize ) );
			VTX_DEBUG( "Max patch vertices: " + std::to_string( glMaxPatchVertices ) );
			VTX_DEBUG( "Max tessellation gen level: " + std::to_string( glMaxTessGenLevel ) );

			VTX_DEBUG( "Max compute work group count: " + std::to_string( glMaxComputeWorkGroupCount[ 0 ] ) + " "
					   + std::to_string( glMaxComputeWorkGroupCount[ 1 ] ) + " "
					   + std::to_string( glMaxComputeWorkGroupCount[ 2 ] ) );
			VTX_DEBUG( "Max compute work group size: " + std::to_string( glMaxComputeWorkGroupSize[ 0 ] ) + " "
					   + std::to_string( glMaxComputeWorkGroupSize[ 1 ] ) + " "
					   + std::to_string( glMaxComputeWorkGroupSize[ 2 ] ) );
			VTX_DEBUG( "Max compute work group invocations: " + std::to_string( glMaxComputeWorkGroupInvocations ) );
		}
	};
} // namespace VTX
#endif
