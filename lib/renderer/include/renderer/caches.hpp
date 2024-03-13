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

			for ( const auto & pair : residueToIndices )
			{
				totalSize += sizeof( uint )
							 * 2; // Assuming each map entry takes sizeof(uint) for key and sizeof(uint) for value
			}

			for ( const auto & pair : residueToPositions )
			{
				totalSize += sizeof( uint )
							 * 2; // Assuming each map entry takes sizeof(uint) for key and sizeof(uint) for value
			}

			for ( const auto & pair : data )
			{
				totalSize
					+= pair.second.size() * sizeof( uint ); // Assuming the elements in the vector are of type uint
			}

			totalSize += representations.size() * sizeof( uchar );

			return totalSize;
		}
	};

} // namespace VTX::Renderer::Cache

#endif
