#ifndef __VTX_REPRESENTATION_TARGET__
#define __VTX_REPRESENTATION_TARGET__

#include "define.hpp"
#include <map>

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

			void compile();

		  private:
			TargetRangeArrays	_atoms	 = TargetRangeArrays();
			TargetRangeElements _bonds	 = TargetRangeElements();
			TargetRangeElements _ribbons = TargetRangeElements();

			TargetRangeMap * _atomsMap	 = new TargetRangeMap();
			TargetRangeMap * _bondsMap	 = new TargetRangeMap();
			TargetRangeMap * _ribbonsMap = new TargetRangeMap();

			void _append( TargetRangeMap & p_range, const uint p_indice, const uint p_count );

			void _mapToRangeArrays( const TargetRangeMap & p_map, TargetRangeArrays & p_rangeArrays );
			void _mapToRangeElements( const TargetRangeMap & p_map, TargetRangeElements & p_rangeElements );
		};
	} // namespace Representation
} // namespace VTX
#endif
