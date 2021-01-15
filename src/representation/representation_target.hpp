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

					if ( _lastIndex == p_firstIndex )
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
				void appendWithBrowse( const std::pair<uint, uint> & p_data )
				{
					appendWithBrowse( p_data.first, p_data.second );
				};
				void appendWithBrowse( const uint p_firstIndex, const uint p_count )
				{
					if ( p_count <= 0 )
						return;

					for ( std::map<uint, uint>::iterator & it = _range.begin(); it != _range.end(); it++ )
					{
						// Already in buffer
						if ( it->first <= p_firstIndex && p_firstIndex < it->first + it->second )
							return;

						if ( ( it->first + it->second ) == p_firstIndex )
						{
							it->second += p_count;

							std::map<uint, uint>::iterator nextIt = it;
							nextIt++;
							if ( nextIt != _range.end() && it->second == nextIt->first )
							{
								it->second += nextIt->second;
								_range.erase( nextIt );
							}

							return;
						}
					}

					_range.emplace( p_firstIndex, p_count );
				}

				const std::map<uint, uint> & getRange() const { return _range; }

			  private:
				std::map<uint, uint> _range		= std::map<uint, uint>();
				uint				 _lastIndex = -2;
				uint				 _lastKey	= -2;
			};

		  public:
			RepresentationTarget() {};

			void appendAtoms( const std::pair<uint, uint> & p_atomData ) { _atoms.appendWithBrowse( p_atomData ); };
			void appendBonds( const std::pair<uint, uint> & p_bondsRange,
							  const std::vector<uint> &		p_indexExtraBondStart,
							  const std::vector<uint> &		p_indexExtraBondEnd )
			{
				_bonds.appendWithBrowse( p_bondsRange );

				for ( uint indexBond : p_indexExtraBondStart )
					_bonds.appendWithBrowse( indexBond * 2, 2 );

				for ( uint indexBond : p_indexExtraBondEnd )
					_bonds.appendWithBrowse( indexBond * 2, 2 );
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
