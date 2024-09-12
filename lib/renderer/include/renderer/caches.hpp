#ifndef __VTX_RENDERER_CACHES__
#define __VTX_RENDERER_CACHES__

#include <map>
#include <util/math/range.hpp>
#include <util/types.hpp>
#include <variant>
#include <vector>

/**
 * @brief Data cached on CPU to avoid recomputation.
 */
namespace VTX::Renderer::Cache
{
	// TODO: auto cache in context?
	// TODO: refacto.

	// template<typename T>
	// using CompressedData = std::variant<T, std::vector<T>>;

	struct SphereCylinder
	{
		Util::Math::Range<size_t> rangeSpheres;
		Util::Math::Range<size_t> rangeCylinders;
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

	struct Ribbon
	{
		Util::Math::Range<size_t>		  range;
		bool							  isEmpty = false;
		std::vector<Vec4f>				  positions;
		std::vector<Vec3f>				  directions;
		std::vector<uchar>				  ssTypes;
		std::vector<uchar>				  colors;
		std::vector<uint>				  ids;
		std::vector<uchar>				  flags;
		std::vector<uint>				  indices;
		std::map<uint, uint>			  residueToIndices;
		std::map<uint, uint>			  residueToPositions;
		std::map<uint, std::vector<uint>> data; // Chain to residues.
		std::vector<uchar>				  representations;

		size_t currentSize() const
		{
			size_t totalSize = sizeof( Ribbon );

			totalSize += positions.size() * sizeof( Vec4f );
			totalSize += directions.size() * sizeof( Vec3f );
			totalSize += ssTypes.size() * sizeof( uchar );
			totalSize += colors.size() * sizeof( uchar );
			totalSize += ids.size() * sizeof( uint );
			totalSize += flags.size() * sizeof( uchar );
			totalSize += indices.size() * sizeof( uint );
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

	struct SES
	{
		bool			   isEmpty = false;
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
