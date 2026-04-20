#ifndef __VTX_RENDERER_GEOMETRY_RIBBON__
#define __VTX_RENDERER_GEOMETRY_RIBBON__

#include "base_geometry.hpp"
#include "renderer/system_data.hpp"
#include "renderer/types.hpp"
#include <core/struct/topology.hpp>
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

		void clear()
		{
			BaseGeometry::clear();
			_construction.clear();
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
			std::vector<Indice>				 indices;
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

			if ( p_data.data.residueSecondaryStructureTypes.empty() )
			{
				cache.isEmpty = true;
				_addRange( p_handle, 0, 0 );
				return;
			}

			std::vector<Construction::Data> &  residues			  = cache.residues;
			std::vector<Indice> &			   bufferIndices	  = cache.indices;
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

				residueToIndices.emplace( usedResidues[ 0 ].index, static_cast<Index>( bufferIndices.size() ) );
				residueToPositions.emplace( usedResidues[ 0 ].index, offset );

				bufferIndices.emplace_back( offset );
				bufferIndices.emplace_back( offset );
				bufferIndices.emplace_back( offset + 1 );
				bufferIndices.emplace_back( offset + 2 );

				for ( Index i = 1; i < static_cast<Index>( usedResidues.size() ) - 2; ++i )
				{
					residueToIndices.emplace( usedResidues[ i ].index, static_cast<Index>( bufferIndices.size() ) );
					residueToPositions.emplace( usedResidues[ i ].index, offset + i );

					bufferIndices.emplace_back( offset + i - 1 );
					bufferIndices.emplace_back( offset + i );
					bufferIndices.emplace_back( offset + i + 1 );
					bufferIndices.emplace_back( offset + i + 2 );
				}

				residues.insert( residues.end(), usedResidues.begin(), usedResidues.end() );
			}

			assert( bufferIndices.size() % 4 == 0 );

			const Index segmentCount = static_cast<Index>( bufferIndices.size() / 4 );

			assert( segmentCount == static_cast<Index>( residueToIndices.size() ) );
			assert( segmentCount == static_cast<Index>( residueToPositions.size() ) );
			assert( static_cast<Index>( residues.size() ) >= segmentCount );

			cache.isEmpty = residues.empty();
			_addRange( p_handle, static_cast<Index>( bufferIndices.size() ), static_cast<Index>( residues.size() ) );

			if ( not cache.isEmpty )
			{
				auto & indiceBuffer = _indices( p_handle );
				indiceBuffer		= bufferIndices;
			}
		}

		void setVisibility( const Desc::Handle p_handle, const Util::Math::BitSet & p_visibility )
		{
			const Construction & cache = _construction.at( p_handle );

			if ( cache.isEmpty )
			{
				return;
			}

			auto & indiceBuffer = _indices( p_handle );
			indiceBuffer.clear();

			for ( const auto & data : cache.residues )
			{
				const Index ca = data.ca;

				if ( not p_visibility.test( ca ) )
				{
					continue;
				}

				const auto it = cache.residueToIndices.find( data.index );
				if ( it == cache.residueToIndices.end() )
				{
					continue;
				}

				const Index offset = it->second;
				indiceBuffer.emplace_back( cache.indices[ offset ] );
				indiceBuffer.emplace_back( cache.indices[ offset + 1 ] );
				indiceBuffer.emplace_back( cache.indices[ offset + 2 ] );
				indiceBuffer.emplace_back( cache.indices[ offset + 3 ] );
			}
		}

	  protected:
		std::map<Desc::Handle, Construction> _construction;
	};

} // namespace VTX::Renderer::Geometry

#endif
