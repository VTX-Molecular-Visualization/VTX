#ifndef __VTX_RENDERER_GEOMETRY_RIBBON__
#define __VTX_RENDERER_GEOMETRY_RIBBON__

#include "base_geometry.hpp"
#include "renderer/system_data.hpp"
#include "renderer/types.hpp"
#include <core/struct/system.hpp>
#include <util/math.hpp>

namespace VTX::Renderer::Geometry
{

	class Ribbon : public BaseGeometry<DrawRangeElements>
	{
	  public:
		struct Construction
		{
			bool isEmpty = false;
			// std::vector<Vec4f>				  positions;
			// std::vector<Vec3f>				  directions;
			// std::vector<uchar>				  ssTypes;
			// std::vector<uchar>				  colors;
			// std::vector<uint>				  ids;
			// std::vector<uchar>				  flags;
			IndexRangeList		   residues;
			std::vector<Index>	   indices;
			std::map<Index, Index> residueToIndices;
			std::map<Index, Index> residueToPositions;

			// std::map<uint, std::vector<uint>> data; // Chain to residues.
			//  std::vector<uchar>				  representations;
		};

		Index sizeItems = 0;

		const Construction & construction( const SystemUID p_uid ) const
		{
			assert( _construction.contains( p_uid ) );

			return _construction[ p_uid ];
		}

		void construct( const SystemData & p_data )
		{
			assert( not _ranges.contains( p_data.uid ) );

			// assert( p_data.data.atomNames.size() == p_data.frame.size() );
			assert( p_data.residueUids.size() == p_data.data.residueSecondaryStructureTypes.size() );
			assert( p_data.residueUids.size() == p_data.residueUids.size() );
			assert( p_data.residueUids.size() == p_data.data.residueFirstAtomIndexes.size() );
			assert( p_data.residueUids.size() == p_data.data.residueAtomCounts.size() );
			assert( p_data.data.chainFirstResidues.size() == p_data.data.chainResidueCounts.size() );

			// Compute data if not cached.
			Construction & cache = _construction[ p_data.uid ];

			if ( cache.isEmpty )
			{
				return;
			}

			// Check if data.
			if ( p_data.data.residueSecondaryStructureTypes.empty() )
			{
				cache.isEmpty = true;
				return;
			}

			// Data to filL.
			IndexRangeList &		 residues			= cache.residues;
			std::vector<Index> &	 bufferIndices		= cache.indices;
			std::map<Index, Index> & residueToIndices	= cache.residueToIndices;
			std::map<Index, Index> & residueToPositions = cache.residueToPositions;

			for ( Index chainIdx : p_data.data.getChainRange() )
			{
				// No enought residues.
				if ( p_data.data.getChainResidueCount( chainIdx ) < 4 )
				{
					continue;
				}

				// Store valid residues.
				std::vector<Index> usedResidues;

				// Find CA and O atoms to validate residues.
				for ( Index residueIdx : p_data.data.getChainResidueRange( chainIdx ) )
				{
					// Find alpha carbon.
					auto optCA = p_data.data.findFirstAtomByName( residueIdx, "CA" );
					// Not an amine acid (water, heme, or phosphate groupment).
					if ( not optCA )
					{
						continue;
					}

					// Find oxygen.
					auto optO = p_data.data.findFirstAtomByName( residueIdx, "O" );
					// Missing oxygen atom.
					if ( not optO )
					{
						continue;
					}

					// Compute direction between carbon alpha and oxygen.
					// const Vec3f & positionCA   = positions[ *CA ];
					// const Vec3f & positionO	   = positions[ *O ];
					// const Vec3f	  directionCAO = Util::Math::normalize( positionO - positionCA );

					// Store residue index for later.
					usedResidues.emplace_back( residueIdx );
				}

				if ( usedResidues.size() < 4 )
				{
					continue;
				}

				// Setup indices mapping.
				residueToPositions.emplace( usedResidues[ 0 ], sizeItems );
				residueToIndices.emplace( usedResidues[ 0 ], static_cast<uint>( bufferIndices.size() ) );

				const Index offset = sizeItems;
				sizeItems++;

				// Add fist segment with duplicate first index to evaluate B-spline at 0-1.
				bufferIndices.emplace_back( offset );
				bufferIndices.emplace_back( offset );
				bufferIndices.emplace_back( offset + 1 );
				bufferIndices.emplace_back( offset + 2 );

				// Add segments.
				for ( uint i = 1; i < usedResidues.size() - 2; ++i )
				{
					residueToPositions.emplace( usedResidues[ i ], static_cast<uint>( sizeItems++ ) );
					residueToIndices.emplace( usedResidues[ i ], static_cast<uint>( bufferIndices.size() ) );

					bufferIndices.emplace_back( offset + i - 1 );
					bufferIndices.emplace_back( offset + i );
					bufferIndices.emplace_back( offset + i + 1 );
					bufferIndices.emplace_back( offset + i + 2 );
				}

				// TODO: better on GPU ?
				// CheckOrientationAndFlip.
				// size_t i;
				// for ( i = 1; i < p_caODirections.size(); ++i )
				//{
				//	if ( Util::Math::dot( p_caODirections[ i ], p_caODirections[ i - 1 ] ) < 0.f )
				//	{
				//		p_caODirections[ i ] = -p_caODirections[ i ];
				//	}
				//}

				// Store added residues.
				residues.addValues( std::span<const Index>( usedResidues.data(), usedResidues.size() - 2 ) );

				// Merge buffers.
				// auto it = data.find( p_chainIdx );
				// if ( it == data.end() )
				//{
				//	data.emplace( p_chainIdx, std::vector<uint>() );
				//}
				// data[ p_chainIdx ].insert(
				//	std::end( data[ p_chainIdx ] ), std::begin( p_residueIndex ), std::end( p_residueIndex )
				//);
			}

			assert( sizeItems == residues.size() );
			assert( sizeItems == bufferIndices.size() / 4 );
			assert( sizeItems == residueToPositions.size() );
			assert( sizeItems == residueToIndices.size() );

			if ( sizeItems == 0 )
			{
				cache.isEmpty = true;
				return;
			}

			addRange( p_data.uid, bufferIndices.size() );
		}

	  protected:
		mutable std::map<SystemUID, Construction> _construction;
	};

} // namespace VTX::Renderer::Geometry

#endif
