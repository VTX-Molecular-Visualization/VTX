#ifndef VTX_RENDERER_STRUCT_INFOS_HPP
#define VTX_RENDERER_STRUCT_INFOS_HPP

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
		int64_t gpuMemoryInfoDedicated {};
		int64_t gpuMemoryInfoTotalAvailable {};
		int64_t gpuMemoryInfoCurrentAvailable {};
		//

		// size_t currentCountBuffers;
		// size_t currentCountTextures;

		// size_t currentSizeBuffers;
		// size_t currentSizeTextures;
		size_t currentSizeCPUCache {};
	};
} // namespace VTX::Renderer

#endif
