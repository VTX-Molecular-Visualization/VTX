#ifndef __VTX_REPRESENTATION_TARGET__
#define __VTX_REPRESENTATION_TARGET__

#include "model/define.hpp"
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
			RepresentationTarget() = default;
			~RepresentationTarget();

			inline const TargetRangeArrays & getAtoms() const
			{
				assert( _atomsMap == nullptr ); // Ensure compiled.
				return _atoms;
			};
			inline const TargetRangeElements & getBonds() const
			{
				assert( _bondsMap == nullptr ); // Ensure compiled.
				return _bonds;
			};
			inline const TargetRangeElements & getRibbons() const
			{
				assert( _ribbonsMap == nullptr ); // Ensure compiled.
				return _ribbons;
			};
			inline const TargetRangeElements & getTrianglesSES() const
			{
				assert( _trianglesSESMap == nullptr ); // Ensure compiled.
				return _trianglesSES;
			};

			inline void appendAtoms( const uint p_indice, const uint p_count )
			{
				_append( *_atomsMap, p_indice, p_count );
			}
			inline void appendBonds( const uint p_indice, const uint p_count )
			{
				_append( *_bondsMap, p_indice, p_count );
			}
			inline void appendRibbons( const uint p_indice, const uint p_count )
			{
				_append( *_ribbonsMap, p_indice, p_count );
			}
			inline void appendTrianglesSES( const uint p_indice, const uint p_count )
			{
				_append( *_trianglesSESMap, p_indice, p_count );
			}
			inline void resetTriangleSES()
			{
				_trianglesSES.indices.clear();
				_trianglesSES.counts.clear();

				_trianglesSESMap = new TargetRangeMap();
			}

			void generate();

		  private:
			TargetRangeArrays	_atoms		  = TargetRangeArrays();
			TargetRangeElements _bonds		  = TargetRangeElements();
			TargetRangeElements _ribbons	  = TargetRangeElements();
			TargetRangeElements _trianglesSES = TargetRangeElements();

			TargetRangeMap * _atomsMap		  = new TargetRangeMap();
			TargetRangeMap * _bondsMap		  = new TargetRangeMap();
			TargetRangeMap * _ribbonsMap	  = new TargetRangeMap();
			TargetRangeMap * _trianglesSESMap = new TargetRangeMap();

			void _append( TargetRangeMap & p_range, const uint p_indice, const uint p_count );

			void _generateArrays( const TargetRangeMap ** const p_map, TargetRangeArrays & p_rangeArrays );
			void _generateElements( const TargetRangeMap ** const p_map, TargetRangeElements & p_rangeElements );

			void _mapToRangeArrays( const TargetRangeMap & p_map, TargetRangeArrays & p_rangeArrays );
			void _mapToRangeElements( const TargetRangeMap & p_map, TargetRangeElements & p_rangeElements );
		};
	} // namespace Representation
} // namespace VTX
#endif
