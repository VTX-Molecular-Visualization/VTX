#include "io/util/bond_recomputation.hpp"
#include <cassert>
#include <util/chrono.hpp>
#include <util/logger.hpp>
#include <util/math.hpp>
#include <util/math/grid.hpp>

namespace
{
	using namespace VTX;
	using namespace VTX::Core::Struct;
	using namespace VTX::Util;
	using namespace VTX::Util::Math;

	// Add a bond to the topology.
	void _addBond( Topology & p_topology, const Index p_firstAtomIndex, const Index p_secondAtomIndex )
	{
		p_topology.bondPairAtomIndexes.emplace_back( p_firstAtomIndex );
		p_topology.bondPairAtomIndexes.emplace_back( p_secondAtomIndex );
		p_topology.bondOrders.emplace_back( Core::ChemDB::Bond::ORDER::SINGLE );
	}

	// Test if two atoms can form a disulfide bond (CYS-SG).
	void _testDisulfideBond(
		Topology &	  p_topology,
		const Frame & p_frame,
		const Index	  p_firstAtomIndex,
		const Index	  p_secondAtomIndex
	)
	{
		constexpr double MAX_DISTANCE_FOR_DISULFIDE_BOND_SQR = 9.0;

		const float sqrDistance = length2( p_frame[ p_firstAtomIndex ] - p_frame[ p_secondAtomIndex ] );
		if ( sqrDistance < MAX_DISTANCE_FOR_DISULFIDE_BOND_SQR )
		{
			_addBond( p_topology, p_firstAtomIndex, p_secondAtomIndex );
		}
	}

	// Recompute disulfide bonds by checking all pairs of atoms in the disulfide grid.
	void _recomputeDisulfides( Topology & p_topology, const Frame & p_frame, const Grid<Index> & p_disulfideGrid )
	{
		for ( const auto & [ cellPosition, cell ] : p_disulfideGrid )
		{
			const size_t cellDenseIndex = p_disulfideGrid.denseIndex( cellPosition );

			p_disulfideGrid.forEachNeighbourCell(
				cellPosition,
				[ & ](
					const Grid<Index>::CellPosition & p_neighbourPosition, const Grid<Index>::Cell & p_neighbourCell
				)
				{
					// Current cell.
					if ( cellPosition == p_neighbourPosition )
					{
						for ( size_t firstIndex = 0; firstIndex < cell.size(); ++firstIndex )
						{
							for ( size_t secondIndex = 0; secondIndex < firstIndex; ++secondIndex )
							{
								_testDisulfideBond( p_topology, p_frame, cell[ firstIndex ], cell[ secondIndex ] );
							}
						}

						return;
					}

					const size_t neighbourDenseIndex = p_disulfideGrid.denseIndex( p_neighbourPosition );

					// Avoid multiples checks.
					if ( cellDenseIndex < neighbourDenseIndex )
					{
						return;
					}

					// Others cells.
					for ( const Index firstAtomIndex : cell )
					{
						for ( const Index secondAtomIndex : p_neighbourCell )
						{
							_testDisulfideBond( p_topology, p_frame, firstAtomIndex, secondAtomIndex );
						}
					}
				}
			);
		}
	}

	// Recompute bonds by checking all pairs of atoms in the candidate atom indexes.
	void _recomputeCandidates(
		Topology &				   p_topology,
		const Frame &			   p_frame,
		const Grid<Index> &		   p_atomGrid,
		const std::vector<Index> & p_candidateAtomIndexes,
		const std::vector<bool> &  p_isCandidateAtom
	)
	{
		constexpr float CANDIDATE_BOND_CUTOFF_SQR = 3.48f * 2.f * 3.48f * 2.f;
		constexpr float MIN_BOND_DISTANCE_SQR	  = 0.03f;

		for ( const Index firstAtomIndex : p_candidateAtomIndexes )
		{
			const Core::ChemDB::Atom::SYMBOL firstAtomSymbol = p_topology.atomSymbols[ firstAtomIndex ];

			p_atomGrid.forEachNeighbourCellAt(
				p_frame[ firstAtomIndex ],
				[ & ]( const Grid<Index>::CellPosition &, const Grid<Index>::Cell & p_neighbourCell )
				{
					for ( const Index secondAtomIndex : p_neighbourCell )
					{
						if ( secondAtomIndex == firstAtomIndex )
						{
							continue;
						}
						if ( p_isCandidateAtom[ secondAtomIndex ] && secondAtomIndex < firstAtomIndex )
						{
							continue;
						}

						const float sqrDistance = length2( p_frame[ firstAtomIndex ] - p_frame[ secondAtomIndex ] );

						if ( sqrDistance > CANDIDATE_BOND_CUTOFF_SQR || sqrDistance < MIN_BOND_DISTANCE_SQR )
						{
							continue;
						}

						const Core::ChemDB::Atom::SYMBOL secondAtomSymbol = p_topology.atomSymbols[ secondAtomIndex ];

						const float atom1Radius
							= Core::ChemDB::Atom::SYMBOL_VDW_RADIUS[ toUnderlying( firstAtomSymbol ) ];
						const float atom2Radius
							= Core::ChemDB::Atom::SYMBOL_VDW_RADIUS[ toUnderlying( secondAtomSymbol ) ];
						const float radiusDistance	  = atom1Radius > atom2Radius ? atom1Radius : atom2Radius;
						const float radiusSqrDistance = radiusDistance * radiusDistance;

						if ( sqrDistance < radiusSqrDistance )
						{
							// Discard H-H.
							if ( firstAtomSymbol != Core::ChemDB::Atom::SYMBOL::A_H
								 || secondAtomSymbol != Core::ChemDB::Atom::SYMBOL::A_H )
							{
								_addBond( p_topology, firstAtomIndex, secondAtomIndex );
							}
						}
					}
				}
			);
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
		std::vector<bool>			 isCandidateAtom( p_frame.size(), false );

		const auto contains = []( const auto & p_values, const auto p_value )
		{ return std::find( p_values.begin(), p_values.end(), p_value ) != p_values.end(); };

		const bool acceptUnknownAtom   = contains( p_filter.atomSymbols, Core::ChemDB::Atom::SYMBOL::UNKNOWN );
		const bool hasCandidateFilters = not p_filter.atomSymbols.empty() || not p_filter.residueSymbols.empty()
										 || not p_filter.categories.empty();

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

			if ( hasCandidateFilters && atomSymbol == Core::ChemDB::Atom::SYMBOL::UNKNOWN && not acceptUnknownAtom )
			{
				continue;
			}

			const bool isCandidate = not hasCandidateFilters || contains( p_filter.atomSymbols, atomSymbol )
									 || contains( p_filter.residueSymbols, residueSymbol )
									 || contains( p_filter.categories, residueCategory );

			if ( isCandidate )
			{
				candidateAtomIndexes.emplace_back( atomIndex );
				isCandidateAtom[ atomIndex ] = true;
			}
		}

		if ( candidateAtomIndexes.empty() && disulfideGrid.getCellCount() == 0 )
		{
			return;
		}

		_recomputeDisulfides( p_topology, p_frame, disulfideGrid );
		_recomputeCandidates( p_topology, p_frame, atomGrid, candidateAtomIndexes, isCandidateAtom );
	}

} // namespace VTX::IO::Util::BondRecomputation
