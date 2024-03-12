#include <array>
#include <string>

namespace VTX::Renderer
{
	struct StructInfos
	{
		long long gpuMemoryInfoDedicated;
		long long gpuMemoryInfoTotalAvailable;
		long long gpuMemoryInfoCurrentAvailable;

		size_t currentSizeBuffers;
		size_t currentSizeTextures;
	};
} // namespace VTX::Renderer
