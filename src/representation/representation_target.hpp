#ifndef __VTX_REPRESENTATION_TARGET__
#define __VTX_REPRESENTATION_TARGET__

#include "define.hpp"
#include "model/category_enum.hpp"
#include <map>
#include <vector>

namespace VTX
{
	namespace Representation
	{
		template<typename T>
		struct TargetRange
		{
			std::vector<T>	  indices;
			std::vector<uint> counts;
		};

		using TargetRangeArrays	  = TargetRange<uint>;
		using TargetRangeElements = TargetRange<void *>;
		using TargetRangeMap	  = std::map<uint, uint>;
		using TargetRangeMapPair  = std::pair<const uint, uint>;

		class RepresentationTarget
		{
		  public:
			RepresentationTarget()	= default;
			~RepresentationTarget() = default;

			inline const TargetRangeArrays & getAtoms() const
			{
				assert( _isGenerated );
				return _atoms;
			}

			inline const TargetRangeElements & getBonds() const
			{
				assert( _isGenerated );
				return _bonds;
			}

			inline const TargetRangeElements & getRibbons() const
			{
				assert( _isGenerated );
				return _ribbons;
			}

			inline const TargetRangeElements & getTrianglesSES( const CATEGORY_ENUM p_category ) const
			{
				assert( _isGenerated );
				// TODO: use [].
				return _trianglesSES.at( p_category );
			}

			inline void appendAtoms( const uint p_indice, const uint p_count )
			{
				_append( _atomsMap, p_indice, p_count );
			}

			inline void appendBonds( const uint p_indice, const uint p_count )
			{
				_append( _bondsMap, p_indice, p_count );
			}

			inline void appendRibbons( const uint p_indice, const uint p_count )
			{
				_append( _ribbonsMap, p_indice, p_count );
			}

			inline void appendTrianglesSES( const CATEGORY_ENUM p_category, const uint p_indice, const uint p_count )
			{
				assert( _trianglesSESMap.find( p_category ) != _trianglesSESMap.end() );
				_append( _trianglesSESMap[ p_category ], p_indice, p_count );
			}

			inline void resetTriangleSES() { _trianglesSES.clear(); }

			void generate();

		  private:
			TargetRangeArrays								   _atoms	= TargetRangeArrays();
			TargetRangeElements								   _bonds	= TargetRangeElements();
			TargetRangeElements								   _ribbons = TargetRangeElements();
			std::map<const CATEGORY_ENUM, TargetRangeElements> _trianglesSES
				= std::map<const CATEGORY_ENUM, TargetRangeElements>();

			TargetRangeMap								  _atomsMap	  = TargetRangeMap();
			TargetRangeMap								  _bondsMap	  = TargetRangeMap();
			TargetRangeMap								  _ribbonsMap = TargetRangeMap();
			std::map<const CATEGORY_ENUM, TargetRangeMap> _trianglesSESMap
				= std::map<const CATEGORY_ENUM, TargetRangeMap>();

			bool _isGenerated = false;
			void _append( TargetRangeMap & p_range, const uint p_indice, const uint p_count );

			void _mapToRangeArrays( TargetRangeMap & p_map, TargetRangeArrays & p_rangeArrays );
			void _mapToRangeElements( TargetRangeMap & p_map, TargetRangeElements & p_rangeElements );
		};
	} // namespace Representation
} // namespace VTX
#endif
