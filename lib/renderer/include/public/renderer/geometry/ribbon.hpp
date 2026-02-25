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

			// Carbon alpha (Ca) positions.
			// Add an extra float increasing along the backbone (to determine direction for two sided ss).
			std ::vector<Vec4f> & bufferCaPositions = cache.positions;
			// Ca -> O directions.
			std::vector<Vec3f> & bufferCaODirections = cache.directions;
			// Secondary structure types.
			std::vector<uchar> &				bufferSSTypes	   = cache.ssTypes;
			std::vector<uchar> &				bufferColors	   = cache.colors;
			std::vector<uint> &					bufferIds		   = cache.ids;
			std::vector<uchar> &				bufferFlags		   = cache.flags;
			std::vector<uint> &					bufferIndices	   = cache.indices;
			std::map<uint, uint> &				residueToIndices   = cache.residueToIndices;
			std::map<uint, uint> &				residueToPositions = cache.residueToPositions;
			std::map<uint, std::vector<uint>> & data			   = cache.data;

			auto _tryConstruct = [ & ](
									 const uint					p_chainIdx,
									 const std::vector<uint> &	p_residueIndex,
									 const std::vector<Vec4f> & p_caPositions,
									 std::vector<Vec3f> &		p_caODirections,
									 const std::vector<uchar> & p_ssTypes,
									 const std::vector<uchar> & p_colors,
									 const std::vector<uint> &	p_ids,
									 const std::vector<uchar> & p_flags

								 )
			{
				if ( p_caPositions.size() >= 4 )
				{
					const size_t nbControlPoints = p_caPositions.size();

					residueToPositions.emplace( p_residueIndex[ 0 ], uint( bufferCaPositions.size() ) );
					residueToIndices.emplace( p_residueIndex[ 0 ], uint( bufferIndices.size() ) );

					const uint offset = uint( bufferCaPositions.size() );

					// Add segment with duplicate first index to evaluate B-spline at 0-1.
					bufferIndices.emplace_back( offset );
					bufferIndices.emplace_back( offset );
					bufferIndices.emplace_back( offset + 1 );
					bufferIndices.emplace_back( offset + 2 );

					for ( uint i = 1; i < nbControlPoints - 2; ++i )
					{
						residueToPositions.emplace( p_residueIndex[ i ], uint( bufferCaPositions.size() + i ) );
						residueToIndices.emplace( p_residueIndex[ i ], uint( bufferIndices.size() ) );

						bufferIndices.emplace_back( offset + i - 1 );
						bufferIndices.emplace_back( offset + i );
						bufferIndices.emplace_back( offset + i + 1 );
						bufferIndices.emplace_back( offset + i + 2 );
					}

					// TODO: better on GPU ?
					// CheckOrientationAndFlip.
					size_t i;
					for ( i = 1; i < p_caODirections.size(); ++i )
					{
						if ( Util::Math::dot( p_caODirections[ i ], p_caODirections[ i - 1 ] ) < 0.f )
						{
							p_caODirections[ i ] = -p_caODirections[ i ];
						}
					}

					// Merge buffers.
					auto it = data.find( p_chainIdx );
					if ( it == data.end() )
					{
						data.emplace( p_chainIdx, std::vector<uint>() );
					}
					data[ p_chainIdx ].insert(
						std::end( data[ p_chainIdx ] ), std::begin( p_residueIndex ), std::end( p_residueIndex )
					);

					bufferCaPositions.insert( bufferCaPositions.end(), p_caPositions.cbegin(), p_caPositions.cend() );
					bufferCaODirections.insert(
						bufferCaODirections.end(), p_caODirections.cbegin(), p_caODirections.cend()
					);
					bufferSSTypes.insert( bufferSSTypes.end(), p_ssTypes.cbegin(), p_ssTypes.cend() );
					bufferColors.insert( bufferColors.end(), p_colors.cbegin(), p_colors.cend() );
					bufferIds.insert( bufferIds.end(), p_ids.cbegin(), p_ids.cend() );
					bufferFlags.insert( bufferFlags.end(), p_flags.cbegin(), p_flags.cend() );
				}
			};

			/////////////////////////
			const std::vector<Vec3f> positions;

			// Temporary vectors, merged with buffers if constructed.
			std::vector<Vec4f> caPositions;
			std::vector<Vec3f> caODirections;
			std::vector<uchar> types;
			std::vector<uchar> colors;
			std::vector<uint>  ids;
			std::vector<uchar> flags;
			std::vector<uint>  residueIndex;

			for ( uint chainIdx = 0; chainIdx < p_data.data.chainFirstResidues.size(); ++chainIdx )
			{
				/*
				const Chain * const chain = _system->getChain( chainIdx );
				if ( chain == nullptr )
				{
					continue;
				}
				*/

				const Index residueCount	= p_data.data.getChainResidueCount( chainIdx );
				const Index idxFirstResidue = p_data.data.getChainFirstResidue( chainIdx );

				// No enought residues.
				if ( residueCount < 4 ) // TODO: what to do ?
				{
					VTX_DEBUG( "Chain residue count < 4" );
					continue;
				}

				bool createVectors = true;
				int	 residueLast   = -1;
				for ( uint residueIdx = idxFirstResidue; residueIdx < idxFirstResidue + residueCount; ++residueIdx )
				{
					if ( createVectors )
					{
						caPositions	  = std::vector<Vec4f>();
						caODirections = std::vector<Vec3f>();
						types		  = std::vector<uint8_t>();
						colors		  = std::vector<ColorIndex>();
						ids			  = std::vector<PickingUID>();
						flags		  = std::vector<Flag>();

						residueIndex = std::vector<uint>();

						createVectors = false;
					}

					/*
					if ( residue == nullptr )
					{
						continue;
					}
					*/

					auto findFirstAtomByName
						= [ &p_data ]( const uint p_residueIdx, const std::string & p_name ) -> std::optional<Index>
					{
						for ( Index i : p_data.data.getResidueAtomRange( p_residueIdx ) )
						{
							if ( ( p_data.data.atomNames )[ i ] == p_name )
							{
								return i;
							}
						}

						return std::nullopt;
					};

					// Use backbone to compute spline data.
					// Find alpha carbon.
					auto CA = findFirstAtomByName( residueIdx, "CA" );

					// Not an amine acid (water, heme, or phosphate groupment).
					if ( not CA ) // TODO: what to do ?
					{
						continue;
					}

					// Find oxygen.
					auto O = findFirstAtomByName( residueIdx, "O" );

					// Missing oxygen atom.
					if ( not O ) // TODO: what to do?
					{
						continue;
					}
					/// TODO: For all these "what to do ?" I think we should render it with spheres or b&s...

					// Compute direction between carbon alpha and oxygen.
					const Vec3f & positionCA   = positions[ *CA ];
					const Vec3f & positionO	   = positions[ *O ];
					const Vec3f	  directionCAO = Util::Math::normalize( positionO - positionCA );

					// Store residue index for later.
					residueIndex.emplace_back( residueIdx );

					// Add carbon alpha (CA) position and CA-O direction.
					caPositions.emplace_back(
						Vec4f( positionCA, float( bufferCaPositions.size() + caPositions.size() ) )
					);
					caODirections.emplace_back( directionCAO );

					// Add secondary structure type.
					types.emplace_back( toUnderlying( p_data.data.getResidueSecondaryStructureType( residueIdx ) ) );

					/*
					switch ( residue->getRepresentation()->getRibbonData().colorMode )
					{
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::JMOL:
						colors.emplace_back( Generic::COLORS_JMOL[ uint( residue->getSecondaryStructure() ) ] );
						break;
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::PROTEIN:
						colors.emplace_back( residue->getSystemPtr()->getColor() );
						break;
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::CUSTOM:
						colors.emplace_back( residue->getRepresentation()->getColor() );
						break;
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::CHAIN:
						colors.emplace_back( residue->getChainPtr()->getColor() );
						break;
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::RESIDUE:
						colors.emplace_back( residue->getColor() );
						break;
					default: colors.emplace_back( Color::Rgba::WHITE ); break;
					}
					*/

					// Generate number between 0 and 255.
					int color = ( ( residueIdx * 7 ) % 256 );
					colors.emplace_back( color );

					// colors.emplace_back( p_data.residueColors[ residueIdx ] );
					ids.emplace_back( p_data.residueUids[ residueIdx ] );

					// Flag.
					// TODO.
					flags.emplace_back( 1 );

					/*
					visibilities.emplace_back( uint(
						_system->isVisible() && chain->isVisible() && residue->isVisible() && CA->isVisible()
						&& O->isVisible()
					) );
					*/

					/*
					if ( residueLast != -1
						 && residue->getIndexInOriginalChain() != residueLast->getIndexInOriginalChain() + 1 )
					{
						_tryConstruct( chainIdx, residueIndex, caPositions, caODirections, types, colors, flags, ids
					); createVectors = true;
					}
					*/

					residueLast = residueIdx;
				}

				// Update buffers and index mapping if SS is constructed.
				_tryConstruct( chainIdx, residueIndex, caPositions, caODirections, types, colors, ids, flags );
			}

			assert( bufferCaPositions.size() == bufferCaODirections.size() );
			assert( bufferCaPositions.size() == bufferSSTypes.size() );
			assert( bufferCaPositions.size() == bufferColors.size() );
			assert( bufferCaPositions.size() == bufferIds.size() );
			assert( bufferCaPositions.size() == bufferFlags.size() );

			if ( bufferCaPositions.empty() )
			{
				cache.isEmpty = true;
				return;
			}

			addRange( p_data.uid, bufferIndices.size() );
			sizeItems += static_cast<Index>( bufferCaPositions.size() );
		}

	  protected:
		mutable std::map<SystemUID, Construction> _construction;
	};

} // namespace VTX::Renderer::Geometry

#endif
