#include <string>

namespace VTX::Renderer::GL
{
	struct StructOpenglInfos
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
		int glMaxUniformBlockSize;
		int glMaxShaderStorageBlockSize;
		int glMaxShaderStorageBufferBindings;

		int gpuMemoryInfoDedicatedVidmemNVX;
		int gpuMemoryInfoTotalAvailableMemoryNVX;
		int gpuMemoryInfoCurrentAvailableVidMemNVX;
	};
} // namespace VTX::Renderer::GL
