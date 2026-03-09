#ifndef __VTX_RENDERER_GEOMETRY_RIBBON__
#define __VTX_RENDERER_GEOMETRY_RIBBON__

#include "base_geometry.hpp"
#include "renderer/system_data.hpp"
#include "renderer/types.hpp"
#include <core/struct/system.hpp>
#include <util/math.hpp>

namespace VTX::Renderer::Geometry
{

	class Ribbon : public BaseGeometry
	{
	  public:
		Ribbon()
		{
			vertexLayout   = "Residues";
			indiceBuffer   = "Index.Ribbons";
			indirectBuffer = "Indirect.Ribbons";
		}

		struct Construction
		{
			bool isEmpty = false;

			struct Data
			{
				Index index;
				Index ca; // Carbon alpha.
				Index o;  // Oxygen.
			};

			std::vector<Data>				 residues;
			std::unordered_map<Index, Index> residueToIndices;
			std::unordered_map<Index, Index> residueToPositions;
		};

		bool empty( const SystemUID p_uid ) const { return _construction.at( p_uid ).isEmpty; }

		const Construction & construction( const Desc::Handle p_handle ) const
		{
			assert( _construction.contains( p_handle ) );
			return _construction.at( p_handle );
		}

		void construct( const Desc::Handle p_handle, const SystemData & p_data )
		{
			assert( p_data.residueUids.size() == p_data.data.residueSecondaryStructureTypes.size() );
			assert( p_data.residueUids.size() == p_data.data.residueFirstAtomIndexes.size() );
			assert( p_data.residueUids.size() == p_data.data.residueAtomCounts.size() );
			assert( p_data.data.chainFirstResidues.size() == p_data.data.chainResidueCounts.size() );

			Construction & cache = _construction[ p_handle ];

			if ( cache.isEmpty )
			{
				return;
			}

			if ( p_data.data.residueSecondaryStructureTypes.empty() )
			{
				cache.isEmpty = true;
				return;
			}

			cache.residues.clear();
			cache.residueToIndices.clear();
			cache.residueToPositions.clear();

			std::vector<Construction::Data> &  residues = cache.residues;
			std::vector<Index>				   bufferIndices;
			std::unordered_map<Index, Index> & residueToIndices	  = cache.residueToIndices;
			std::unordered_map<Index, Index> & residueToPositions = cache.residueToPositions;

			for ( Index chainIdx : p_data.data.getChainRange() )
			{
				if ( p_data.data.getChainResidueCount( chainIdx ) < 4 )
				{
					continue;
				}

				std::vector<Construction::Data> usedResidues;
				usedResidues.reserve( p_data.data.getChainResidueCount( chainIdx ) );

				for ( Index residueIdx : p_data.data.getChainResidueRange( chainIdx ) )
				{
					const auto optCA = p_data.data.findFirstAtomByName( residueIdx, "CA" );
					if ( not optCA )
					{
						continue;
					}

					const auto optO = p_data.data.findFirstAtomByName( residueIdx, "O" );
					if ( not optO )
					{
						continue;
					}

					usedResidues.emplace_back( residueIdx, *optCA, *optO );
				}

				if ( usedResidues.size() < 4 )
				{
					continue;
				}

				const Index offset = static_cast<Index>( residues.size() );

				residueToPositions.emplace( usedResidues[ 0 ].index, offset );
				residueToIndices.emplace( usedResidues[ 0 ].index, static_cast<Index>( bufferIndices.size() ) );

				bufferIndices.emplace_back( offset );
				bufferIndices.emplace_back( offset );
				bufferIndices.emplace_back( offset + 1 );
				bufferIndices.emplace_back( offset + 2 );

				for ( Index i = 1; i < static_cast<Index>( usedResidues.size() ) - 2; ++i )
				{
					residueToPositions.emplace( usedResidues[ i ].index, offset + i );
					residueToIndices.emplace( usedResidues[ i ].index, static_cast<Index>( bufferIndices.size() ) );

					bufferIndices.emplace_back( offset + i - 1 );
					bufferIndices.emplace_back( offset + i );
					bufferIndices.emplace_back( offset + i + 1 );
					bufferIndices.emplace_back( offset + i + 2 );
				}

				residues.insert( residues.end(), usedResidues.begin(), usedResidues.end() );
			}

			assert( bufferIndices.size() % 4 == 0 );

			const Index segmentCount = static_cast<Index>( bufferIndices.size() / 4 );

			assert( segmentCount == static_cast<Index>( residueToPositions.size() ) );
			assert( segmentCount == static_cast<Index>( residueToIndices.size() ) );
			assert( static_cast<Index>( residues.size() ) >= segmentCount );

			cache.isEmpty = residues.empty();
			if ( not cache.isEmpty )
			{
				_addRange(
					p_handle, static_cast<Index>( bufferIndices.size() ), static_cast<Index>( residues.size() )
				);

				auto & indiceBuffer = _indices( p_handle );
				indiceBuffer		= std::move( bufferIndices );
			}
		}

	  protected:
		std::map<Desc::Handle, Construction> _construction;
	};

} // namespace VTX::Renderer::Geometry

#endif
