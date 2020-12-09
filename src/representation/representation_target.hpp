#ifndef __VTX_REPRESENTATION_TARGET__
#define __VTX_REPRESENTATION_TARGET__

#ifdef _MSC_VER
#pragma once
#endif

#include <map>

namespace VTX
{
	namespace Representation
	{
		struct RepresentationTarget
		{
		  private:
			struct TargetRange
			{
				TargetRange() {};

				void append( const std::pair<uint, uint> & p_data ) { append( p_data.first, p_data.second ); };
				void append( const uint p_firstIndex, const uint p_count )
				{
					if ( p_count <= 0 )
						return;

					if ( _lastIndex + 1 == p_firstIndex )
					{
						_range[ _lastKey ] += p_count;
						_lastIndex += p_count;
					}
					else
					{
						_range.emplace( p_firstIndex, p_count );

						_lastKey   = p_firstIndex;
						_lastIndex = p_firstIndex + p_count;
					}
				}
				const std::map<uint, uint> & getRange() const { return _range; }

			  private:
				std::map<uint, uint> _range		= std::map<uint, uint>();
				uint				 _lastIndex = -2;
				uint				 _lastKey	= -2;
			};

		  public:
			RepresentationTarget() {};

			void appendAtoms( const std::pair<uint, uint> & p_atomData ) { _atoms.append( p_atomData ); };
			void appendBonds( const std::pair<uint, uint> & p_bondsRange, const std::vector<uint> & p_indexExtraBondStart, const std::vector<uint> & p_indexExtraBondEnd )
			{
				_bonds.append( p_bondsRange );

				for ( uint indexBond : p_indexExtraBondStart )
					_bonds.append( indexBond * 2, 2 );

				for ( uint indexBond : p_indexExtraBondEnd )
					_bonds.append( indexBond * 2, 2 );
			};
			void appendRibbons( const std::pair<uint, uint> & p_ribbonRange ) { _ribbons.append( p_ribbonRange ); }

			const std::map<uint, uint> & getAtoms() const { return _atoms.getRange(); };
			const std::map<uint, uint> & getBonds() const { return _bonds.getRange(); };
			const std::map<uint, uint> & getRibbons() const { return _ribbons.getRange(); };

		  private:
			TargetRange _atoms	 = TargetRange();
			TargetRange _bonds	 = TargetRange();
			TargetRange _ribbons = TargetRange();
		};
	} // namespace Representation
} // namespace VTX
#endif
