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
		struct TargetRange
		{
			std::vector<uint> indices = std::vector<uint>();
			std::vector<uint> counts  = std::vector<uint>();
		};

		class RepresentationTarget
		{
		  public:
			RepresentationTarget() = default;

			inline const TargetRange & getAtoms() const { return _atoms; };
			inline const TargetRange & getBonds() const { return _bonds; };
			inline const TargetRange & getRibbons() const { return _ribbons; };

			inline void appendAtoms( const uint p_index, const uint p_count ) { _append( _atoms, p_index, p_count ); }
			inline void appendBonds( const uint p_index, const uint p_count ) { _append( _bonds, p_index, p_count ); }
			inline void appendRibbons( const uint p_index, const uint p_count )
			{
				_append( _ribbons, p_index, p_count );
			}

		  private:
			TargetRange _atoms	 = TargetRange();
			TargetRange _bonds	 = TargetRange();
			TargetRange _ribbons = TargetRange();

			void _append( TargetRange & p_range, const uint p_index, const uint p_count )
			{
				/*
				// Init.
				if ( p_range.second == 0 )
				{
					p_range.first  = p_index;
					p_range.second = p_count;
				}
				// Extend lower bound.
				else if ( p_index <= p_range.first )
				{
					p_range.second += ( p_range.first - p_index );
					p_range.first = p_index;
				}
				// Extend upper bound.
				else if ( p_index >= p_range.first + p_range.second )
				{
					p_range.second = p_index + p_count - p_range.first;
				}
				// Range already contained.
				else
				{
				}
				*/
			}
		};
	} // namespace Representation
} // namespace VTX
#endif
