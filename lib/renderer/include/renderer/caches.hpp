#ifndef __VTX_RENDERER_CACHES__
#define __VTX_RENDERER_CACHES__

#include <map>
#include <util/types.hpp>
#include <vector>

namespace VTX::Renderer::Cache
{
	// TODO: auto cache in context?

	struct RangedCache
	{
		size_t offset = 0;
		size_t size	  = 0;
	};

	struct SphereCylinder : public RangedCache
	{
		std::vector<uchar> flags;
	};

	struct Ribbon : public RangedCache
	{
		bool							  isEmpty = false;
		std::vector<Vec4f>				  bufferCaPositions;
		std::vector<Vec3f>				  bufferCaODirections;
		std::vector<uchar>				  bufferSSTypes;
		std::vector<uchar>				  bufferColors;
		std::vector<uint>				  bufferIds;
		std::vector<uchar>				  bufferFlags;
		std::vector<uint>				  bufferIndices;
		std::map<uint, uint>			  residueToIndices;
		std::map<uint, uint>			  residueToPositions;
		std::map<uint, std::vector<uint>> data; // Chain to residues.
	};

} // namespace VTX::Renderer::Cache

#endif
