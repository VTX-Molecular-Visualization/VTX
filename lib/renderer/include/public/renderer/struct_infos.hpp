#ifndef __VTX_STRUCT_INFOS__
#define __VTX_STRUCT_INFOS__

#include <array>
#include <string>

namespace VTX::Renderer
{
	/**
	 * @brief Store renderer informations.
	 */
	struct StructInfos
	{
		std::string renderer;

		// Can be undefined.
		long long gpuMemoryInfoDedicated;
		long long gpuMemoryInfoTotalAvailable;
		long long gpuMemoryInfoCurrentAvailable;
		//

		size_t currentCountBuffers;
		size_t currentCountTextures;

		size_t currentSizeBuffers;
		size_t currentSizeTextures;
		size_t currentSizeCPUCache;
	};
} // namespace VTX::Renderer

#endif
