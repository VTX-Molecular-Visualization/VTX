#ifndef __VTX_STRUCT_INFOS__
#define __VTX_STRUCT_INFOS__

#include <array>
#include <cstddef>
#include <string>

namespace VTX::Renderer
{
	/**
	 * @brief Store renderer informations.
	 */
	struct StructInfos
	{
		std::string renderer;
		bool		cudaAvailable  = false;
		size_t		maxTextureSize = 0;

		// Can be undefined.
		long long gpuMemoryInfoDedicated		= 0;
		long long gpuMemoryInfoTotalAvailable	= 0;
		long long gpuMemoryInfoCurrentAvailable = 0;
		//

		// size_t currentCountBuffers;
		// size_t currentCountTextures;

		// size_t currentSizeBuffers;
		// size_t currentSizeTextures;
		size_t currentSizeCPUCache = 0;
	};
} // namespace VTX::Renderer

#endif
