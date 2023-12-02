#include <array>
#include <string>

namespace VTX::Renderer::Context::GL
{
	enum E_GL_EXTENSIONS
	{
		NVX_gpu_memory_info = 0,
		EXTENSIONS_COUNT
	};

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

		std::array<bool, E_GL_EXTENSIONS::EXTENSIONS_COUNT> glExtensions = { false };
	};
} // namespace VTX::Renderer::Context::GL
