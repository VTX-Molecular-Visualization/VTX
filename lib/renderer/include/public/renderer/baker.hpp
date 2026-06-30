#ifndef __VTX_RENDERER_BAKER__
#define __VTX_RENDERER_BAKER__

#include <cstdint>
#include <util/filesystem.hpp>
#include <vector>

namespace VTX::Renderer::Baker
{
	constexpr uint32_t DEFAULT_ENVIRONMENT_FACE_SIZE = 1024;

	/**
	 * @brief Look for .hdr or .exr.
	 */
	bool isEnvironmentMapFile( const FilePath & p_path );

	/**
	 * @brief Build a cubemap from an equirectangular environment map.
	 */
	std::vector<uint16_t> buildEnvironmentCubemap( const FilePath & p_path, const uint32_t p_faceSize );

	/**
	 * @brief Bake an environment map to a KTX file.
	 */
	bool bakeEnvironmentMapToKtx( const FilePath & p_path, const uint32_t p_faceSize );

	/**
	 * @brief Bake all environment maps in a directory to KTX files.
	 */
	int bakeEnvironmentDirectoryToKtx( const FilePath & p_hdrRoot, const uint32_t p_faceSize );
} // namespace VTX::Renderer::Baker

#endif
