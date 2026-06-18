#include "io/util/bond_recomputation.hpp"
#include <cassert>
#include <util/chrono.hpp>
#include <util/logger.hpp>
#include <util/math/grid.hpp>

namespace
{
	using namespace VTX;
	using namespace VTX::Core::Struct;
	using namespace VTX::Util;
	using namespace VTX::Util::Math;

	constexpr double MAX_DISTANCE_FOR_DISULFIDE_BOND_SQR = 9.0;

	void _recomputeDisulfides( Topology & p_topology, const Frame & p_frame, const Grid<Index> & p_cellList )
	{
		for ( const size_t sulfurAtom1 : p_sulfurAtoms )
		{
			const std::vector<size_t> & neighbours = p_cellList.getNeighbours( p_frame.positions()[ sulfurAtom1 ] );

			for ( const size_t neighbour : neighbours )
			{
				const std::vector<size_t> & sulfurVectorInNeighbour = p_cellList.getCysteineSulfurAtoms( neighbour );

				for ( const size_t sulfurAtom2 : sulfurVectorInNeighbour )
				{
					if ( sulfurAtom1 <= sulfurAtom2 )
					{
						continue;
					}

					const double sqrDist
						= _sqrDistance( p_frame.positions()[ sulfurAtom1 ], p_frame.positions()[ sulfurAtom2 ] );

					if ( sqrDist < MAX_SQR_DISTANCE_FOR_DISULFIDE_BOND )
					{
						p_frame.add_bond( sulfurAtom1, sulfurAtom2 );
					}
				}
			}
		}
	}

} // namespace

namespace VTX::IO::Util::BondRecomputation
{

	void recomputeBonds(
		VTX::Core::Struct::Topology &	 p_topology,
		const VTX::Core::Struct::Frame & p_frame,
		const BondRecomputeFilter &		 p_filter
	)
	{
		VTX::Util::ScopedChrono chrono( "BondRecomputation::recomputeBonds" );
		VTX_INFO( "Recomputing bonds..." );

		assert( p_frame.size() == p_topology.getAtomCount() );

		VTX::Util::Math::Grid<Index> atomGrid;
		VTX::Util::Math::Grid<Index> disulfideGrid;
		std::vector<Index>			 candidateAtomIndexes;

		const auto matches = []( const auto & p_values, const auto p_value )
		{ return p_values.empty() || std::find( p_values.begin(), p_values.end(), p_value ) != p_values.end(); };

		const bool acceptUnknownAtom
			= std::find( p_filter.atomSymbols.begin(), p_filter.atomSymbols.end(), Core::ChemDB::Atom::SYMBOL::UNKNOWN )
			  != p_filter.atomSymbols.end();

		// Loop over atoms to find canditates.
		for ( Index atomIndex = 0; atomIndex < p_frame.size(); ++atomIndex )
		{
			// Sort atoms in grid.
			atomGrid.add( atomIndex, p_frame[ atomIndex ] );

			const Index residueIndex = p_topology.atomResidueIndexes[ atomIndex ];
			if ( residueIndex >= p_topology.getResidueCount() )
			{
				continue;
			}

			const Core::ChemDB::Atom::SYMBOL	atomSymbol		= p_topology.atomSymbols[ atomIndex ];
			const Core::ChemDB::Residue::SYMBOL residueSymbol	= p_topology.residueSymbols[ residueIndex ];
			const Core::ChemDB::Category::TYPE	residueCategory = p_topology.residueCategories[ residueIndex ];

			// Check for disulfide bonds (CYS-SG).
			if ( p_filter.includeDisulfides && residueSymbol == Core::ChemDB::Residue::SYMBOL::CYS
				 && atomSymbol == Core::ChemDB::Atom::SYMBOL::A_S )
			{
				disulfideGrid.add( atomIndex, p_frame[ atomIndex ] );
			}

			if ( atomSymbol == Core::ChemDB::Atom::SYMBOL::UNKNOWN && not acceptUnknownAtom )
			{
				continue;
			}

			if ( matches( p_filter.atomSymbols, atomSymbol ) && matches( p_filter.residueSymbols, residueSymbol )
				 && matches( p_filter.categories, residueCategory ) )
			{
				candidateAtomIndexes.emplace_back( atomIndex );
			}
		}

		if ( candidateAtomIndexes.empty() && disulfideGrid.getCellCount() == 0 )
		{
			return;
		}

		_recomputeDisulfides( p_topology, p_frame, disulfideGrid );
		//_recomputeBondsOfNonStandardResidues( p_frame, cellList );
	}

	/*
	void BondRecomputation::_recomputeBondsOfNonStandardResidues(
		chemfiles::Frame & frame,
		const CellList &   p_cellList
	)
	{
		const double cutoff				 = 3.48 * 2.;
		const double cutoffPow2			 = cutoff * cutoff;
		const int	 hydrogenSymbolValue = int( VTX::Core::ChemDB::Atom::SYMBOL::A_H );

		const std::vector<std::vector<size_t>> & atomsToCheck = p_cellList.getNonStdAtoms();

		for ( size_t cellIndex = 0; cellIndex < atomsToCheck.size(); cellIndex++ )
		{
			const size_t atomsInCell = atomsToCheck[ cellIndex ].size();

			for ( size_t nghb = 0; nghb < p_cellList.getNeighbourList()[ cellIndex ].size(); nghb++ )
			{
				const size_t neighborCellIndex = p_cellList.getNeighbourList()[ cellIndex ][ nghb ];
				const size_t atomNumInCell	   = p_cellList.getCellList()[ neighborCellIndex ].size();

				// const bool selfCell = ( cellIndex == neighborCellIndex );

				for ( size_t i = 0; i < atomsInCell; i++ )
				{
					const size_t			indexAtom1	= atomsToCheck[ cellIndex ][ i ];
					const chemfiles::Atom & atom1		= frame.topology()[ indexAtom1 ];
					const int				symbolAtom1 = int( atom1.atomic_number().value_or( 0 ) );

					for ( size_t j = 0; j < atomNumInCell; j++ )
					{
						const size_t indexAtom2 = p_cellList.getCellList()[ neighborCellIndex ][ j ];

						const double interAtomicSqrDist
							= _sqrDistance( frame.positions()[ indexAtom1 ], frame.positions()[ indexAtom2 ] );

						// Perform distance test and ignore atoms with almost the same coordinates
						if ( ( interAtomicSqrDist > cutoffPow2 ) || ( interAtomicSqrDist < 0.03 ) )
						{
							continue;
						}

						const chemfiles::Atom & atom2		= frame.topology()[ indexAtom2 ];
						const int				symbolAtom2 = int( atom2.atomic_number().value_or( 0 ) );

						const float atom1Radius		  = VTX::Core::ChemDB::Atom::SYMBOL_VDW_RADIUS[ symbolAtom1 ];
						const float atom2Radius		  = VTX::Core::ChemDB::Atom::SYMBOL_VDW_RADIUS[ symbolAtom2 ];
						const float radiusDistance	  = atom1Radius > atom2Radius ? atom1Radius : atom2Radius;
						const float radiusSqrDistance = radiusDistance * radiusDistance;

						if ( interAtomicSqrDist < radiusSqrDistance )
						{
							// Prevent hydrogen atoms from bonding with each other
							if ( symbolAtom1 != hydrogenSymbolValue || symbolAtom2 != hydrogenSymbolValue )
							{
								frame.add_bond( indexAtom1, indexAtom2 );
							}
						}
					}
				}
			}
		}
	}
	*/

} // namespace VTX::IO::Util::BondRecomputation
