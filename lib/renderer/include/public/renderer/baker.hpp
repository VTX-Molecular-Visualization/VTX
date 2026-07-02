#ifndef __VTX_RENDERER_BAKER__
#define __VTX_RENDERER_BAKER__

#include <util/filesystem.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Renderer::Baker
{
	using EnvironmentTexel = ushort;

	constexpr uint DEFAULT_ENVIRONMENT_FACE_SIZE = 1024;

	/**
	 * @brief Look for .hdr or .exr.
	 */
	bool isEnvironmentMapFile( const FilePath & p_path );

	/**
	 * @brief Build a cubemap from an equirectangular environment map.
	 */
	std::vector<EnvironmentTexel> buildEnvironmentCubemap( const FilePath & p_path, const uint p_faceSize );

	/**
	 * @brief Load a cubemap from a KTX file.
	 */
	std::vector<EnvironmentTexel> loadEnvironmentCubemapKtx( const FilePath & p_path, const uint p_faceSize );

	/**
	 * @brief Bake an environment map to a KTX file.
	 */
	bool bakeEnvironmentMapToKtx( const FilePath & p_path, const FilePath & p_outputPath, const uint p_faceSize );
} // namespace VTX::Renderer::Baker

#endif
