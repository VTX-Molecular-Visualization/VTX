#ifndef __VTX_RENDERER_CACHES__
#define __VTX_RENDERER_CACHES__

#include <map>
#include <util/types.hpp>
#include <variant>
#include <vector>

namespace VTX::Renderer::Cache
{
	// TODO: auto cache in context?
	// TODO: refacto.

	// template<typename T>
	// using CompressedData = std::variant<T, std::vector<T>>;

	struct RangedCache
	{
		size_t offset = 0;
		size_t size	  = 0;
	};

	struct SphereCylinder : public RangedCache
	{
		// TODO: optimize when same data.
		std::vector<uchar> flags;
		std::vector<uchar> representations;

		size_t currentSize() const
		{
			size_t totalSize = sizeof( SphereCylinder );

			totalSize += flags.size() * sizeof( uchar );
			totalSize += representations.size() * sizeof( uchar );

			return totalSize;
		}
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
		std::vector<uchar>				  representations;

		size_t currentSize() const
		{
			size_t totalSize = sizeof( Ribbon );

			totalSize += bufferCaPositions.size() * sizeof( Vec4f );
			totalSize += bufferCaODirections.size() * sizeof( Vec3f );
			totalSize += bufferSSTypes.size() * sizeof( uchar );
			totalSize += bufferColors.size() * sizeof( uchar );
			totalSize += bufferIds.size() * sizeof( uint );
			totalSize += bufferFlags.size() * sizeof( uchar );
			totalSize += bufferIndices.size() * sizeof( uint );
			totalSize += residueToIndices.size() * ( sizeof( uint ) * 2 );
			totalSize += residueToPositions.size() * ( sizeof( uint ) * 2 );

			for ( const auto & pair : data )
			{
				totalSize += ( pair.second.size() + 1 ) * sizeof( uint );
			}

			totalSize += representations.size() * sizeof( uchar );

			return totalSize;
		}
	};

	struct SES : public RangedCache
	{
		std::vector<Vec3f> positions;
		std::vector<uchar> colors;
		std::vector<uint>  ids;
		std::vector<uchar> flags;
		std::vector<uint>  indices;
		std::vector<uchar> representations;

		size_t currentSize() const
		{
			size_t totalSize = sizeof( SES );

			totalSize += positions.size() * sizeof( Vec3f );
			totalSize += colors.size() * sizeof( uchar );
			totalSize += ids.size() * sizeof( uint );
			totalSize += flags.size() * sizeof( uchar );
			totalSize += indices.size() * sizeof( uint );
			totalSize += representations.size() * sizeof( uchar );

			return totalSize;
		}
	};

} // namespace VTX::Renderer::Cache

#endif
