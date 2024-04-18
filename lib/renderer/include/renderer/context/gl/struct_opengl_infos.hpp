#include <array>
#include <string>

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
	};
} // namespace VTX::Renderer::Context::GL
