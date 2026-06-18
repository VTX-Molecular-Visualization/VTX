#include "io/util/bond_order_guessing.hpp"
#include <cstdint>
#include <util/chrono.hpp>
#include <util/logger.hpp>
#include <util/math.hpp>
#include <vector>

namespace VTX::IO::Util
{
	using namespace VTX::Core::ChemDB::Atom;
	using namespace VTX::Core::ChemDB::Bond;
	using namespace VTX::Core::Struct;
	using namespace VTX::Util::Math;

	namespace
	{
		// Cycle classification used to decide aromaticity heuristics.
		enum class CycleState : uint8_t
		{
			None,
			Cyclic,
			Planar,
		};

		// Neighbour atom data grouped by element for local bond order rules.
		struct AtomData
		{
			Index index	   = 0;
			Vec3f position = Vec3f( 0.f );
			float distance = 0.f;
		};

		// Adjacency entry carrying both neighbour atom and source bond indexes.
		struct LinkedAtom
		{
			Index atomIndex = 0;
			Index bondIndex = 0;
		};

		// Element-classified neighbours of the atom currently being checked.
		struct NeighbourData
		{
			NeighbourData()
			{
				oxygens.reserve( 4 );
				carbons.reserve( 2 );
				nitrogens.reserve( 3 );
			}

			std::vector<AtomData> oxygens;
			std::vector<AtomData> carbons;
			std::vector<AtomData> nitrogens;
		};

		using NeighbourIndexes = std::vector<std::vector<LinkedAtom>>;

		// Test if an atom has the expected element symbol.
		bool _isSymbol( const Topology & p_topology, const Index p_atomIndex, const SYMBOL p_symbol ) noexcept
		{ return p_topology.getAtomSymbol( p_atomIndex ) == p_symbol; }

		// Test if an atom symbol can participate in planar aromatic rings.
		bool _isConjugableRingAtom( const SYMBOL p_symbol ) noexcept
		{
			return p_symbol == SYMBOL::A_C || p_symbol == SYMBOL::A_N || p_symbol == SYMBOL::A_O
				   || p_symbol == SYMBOL::A_S;
		}

		// Build adjacency lists while detecting whether any bond order is unknown.
		bool _buildNeighbourStruct( const Topology & p_topology, NeighbourIndexes & p_linkedAtomsVector )
		{
			p_linkedAtomsVector.clear();
			p_linkedAtomsVector.resize( p_topology.getAtomCount() );

			bool hasUnknownBondOrders = false;

			for ( Index bondIndex = 0; bondIndex < p_topology.getBondCount(); ++bondIndex )
			{
				const Index firstAtomIndex	= p_topology.getBondFirstAtom( bondIndex );
				const Index secondAtomIndex = p_topology.getBondSecondAtom( bondIndex );

				hasUnknownBondOrders = hasUnknownBondOrders || p_topology.bondOrders[ bondIndex ] == ORDER::UNKNOWN;

				if ( _isSymbol( p_topology, firstAtomIndex, SYMBOL::A_H )
					 || _isSymbol( p_topology, secondAtomIndex, SYMBOL::A_H ) )
				{
					continue;
				}

				p_linkedAtomsVector[ firstAtomIndex ].emplace_back( LinkedAtom { secondAtomIndex, bondIndex } );
				p_linkedAtomsVector[ secondAtomIndex ].emplace_back( LinkedAtom { firstAtomIndex, bondIndex } );
			}

			return hasUnknownBondOrders;
		}

		// Estimate local planarity around a central atom from cross-product alignment.
		float _computeAverageCenterDotCross( const Frame & p_frame, const std::vector<Index> & p_atoms )
		{
			const size_t atomCount = p_atoms.size();
			if ( atomCount < 3 )
			{
				return 0.f;
			}

			std::vector<Vec3f> positions;
			positions.resize( atomCount + 1 );

			for ( size_t i = 0; i < atomCount; i++ )
			{
				positions[ i ] = p_frame[ p_atoms[ i ] ];
			}
			positions[ atomCount ] = positions[ 1 ];

			std::vector<Vec3f> crossProducts;
			crossProducts.resize( atomCount + 1 );

			for ( size_t i = 1; i < atomCount; i++ )
			{
				const Vec3f vec0 = safeNormalize( positions[ i ] - positions[ 0 ] );
				const Vec3f vec1 = safeNormalize( positions[ i + 1 ] - positions[ 0 ] );

				crossProducts[ i ] = safeNormalize( cross( vec0, vec1 ) );

				if ( i > 1 && dot( crossProducts[ i - 1 ], crossProducts[ i ] ) < 0.f )
				{
					crossProducts[ i ] = -crossProducts[ i ];
				}
			}
			crossProducts[ atomCount ] = crossProducts[ 1 ];

			float avg = 0.f;
			for ( size_t i = 1; i < atomCount; i++ )
			{
				avg += dot( crossProducts[ i ], crossProducts[ i + 1 ] );
			}

			return avg / float( atomCount - 1 );
		}

		// Estimate ring planarity from consecutive cross-product alignment.
		float _computeAverageRingDotCross(
			const Frame &			   p_frame,
			const std::vector<Index> & p_atoms,
			const size_t			   p_atomCount,
			Vec3f &					   p_dir
		)
		{
			std::vector<Vec3f> positions;
			positions.resize( p_atomCount + 2 );

			for ( size_t i = 0; i < p_atomCount; i++ )
			{
				positions[ i ] = p_frame[ p_atoms[ i ] ];
			}

			positions[ p_atomCount ]	 = positions[ 0 ];
			positions[ p_atomCount + 1 ] = positions[ 1 ];

			std::vector<Vec3f> crossProducts;
			crossProducts.resize( p_atomCount + 1 );

			for ( size_t i = 0; i < p_atomCount; i++ )
			{
				const Vec3f vec0 = safeNormalize( positions[ i ] - positions[ i + 1 ] );
				const Vec3f vec1 = safeNormalize( positions[ i + 2 ] - positions[ i + 1 ] );

				crossProducts[ i ] = safeNormalize( cross( vec0, vec1 ) );

				if ( i > 1 && dot( crossProducts[ i - 1 ], crossProducts[ i ] ) < 0.f )
				{
					crossProducts[ i ] = -crossProducts[ i ];
				}

				p_dir += crossProducts[ i ];
			}

			crossProducts[ p_atomCount ] = crossProducts[ 0 ];

			float avg = 0.f;
			for ( size_t i = 0; i < p_atomCount; i++ )
			{
				avg += dot( crossProducts[ i ], crossProducts[ i + 1 ] );
			}

			return avg / float( p_atomCount );
		}

		// Check that ring bonds stay close to the candidate aromatic plane.
		bool _verifyPlanarBonds(
			const Topology &		   p_topology,
			const Frame &			   p_frame,
			const std::vector<Index> & p_atoms,
			const size_t			   p_atomCount,
			const NeighbourIndexes &   p_linkedAtomsVector,
			const Vec3f &			   p_dir,
			const float				   p_cutoff
		)
		{
			for ( size_t i = 0; i < p_atomCount; i++ )
			{
				const Index	 atomIndex	= p_atoms[ i ];
				const SYMBOL atomSymbol = p_topology.getAtomSymbol( atomIndex );
				if ( not _isConjugableRingAtom( atomSymbol ) )
				{
					continue;
				}

				const Vec3f & atomPos = p_frame[ atomIndex ];

				for ( const LinkedAtom & neighbour : p_linkedAtomsVector[ atomIndex ] )
				{
					const Index	 neighbourIndex	 = neighbour.atomIndex;
					const SYMBOL neighbourSymbol = p_topology.getAtomSymbol( neighbourIndex );
					if ( not _isConjugableRingAtom( neighbourSymbol ) )
					{
						continue;
					}

					const Vec3f vec		   = safeNormalize( p_frame[ neighbourIndex ] - atomPos );
					float		dotProduct = dot( vec, p_dir );
					dotProduct			   = dotProduct < 0.f ? -dotProduct : dotProduct;

					if ( dotProduct > p_cutoff )
					{
						return false;
					}
				}
			}

			return true;
		}

		// Recursively search small cycles and tag planar ones.
		void _tagCyclesRecursive(
			const Topology &		  p_topology,
			const Frame &			  p_frame,
			const NeighbourIndexes &  p_linkedAtomsVector,
			std::vector<CycleState> & p_cycleStatePerAtoms,
			std::vector<Index> &	  p_cycleIndexes,
			const short				  p_counter
		)
		{
			const Index currentAtomIndex = p_cycleIndexes[ p_counter - 1 ];
			for ( const LinkedAtom & nextAtom : p_linkedAtomsVector[ currentAtomIndex ] )
			{
				const Index nextAtomIndex		   = nextAtom.atomIndex;
				bool		nextAtomAlreadyInCycle = false;
				for ( int i = 0; i < p_counter - 1; i++ )
				{
					nextAtomAlreadyInCycle = nextAtomAlreadyInCycle || p_cycleIndexes[ i ] == nextAtomIndex;
				}

				if ( nextAtomAlreadyInCycle )
				{
					if ( p_counter >= 5 && nextAtomIndex == p_cycleIndexes[ 0 ] )
					{
						Vec3f		dir( 0.f );
						const float avgDotCross
							= _computeAverageRingDotCross( p_frame, p_cycleIndexes, p_counter, dir );
						const bool isPlanar
							= avgDotCross > 0.96f
							  && _verifyPlanarBonds(
								  p_topology, p_frame, p_cycleIndexes, p_counter, p_linkedAtomsVector, dir, 0.35f
							  );

						const CycleState cycleState = isPlanar ? CycleState::Planar : CycleState::Cyclic;

						for ( int i = 0; i < p_counter; i++ )
						{
							p_cycleStatePerAtoms[ p_cycleIndexes[ i ] ] = cycleState;
						}
					}
				}
				else if ( p_counter < 6 )
				{
					p_cycleIndexes[ p_counter ] = nextAtomIndex;
					_tagCyclesRecursive(
						p_topology, p_frame, p_linkedAtomsVector, p_cycleStatePerAtoms, p_cycleIndexes, p_counter + 1
					);
				}
			}
		}

		// Tag atoms that belong to cyclic or planar cyclic structures.
		void _tagCycles(
			const Topology &		  p_topology,
			const Frame &			  p_frame,
			const NeighbourIndexes &  p_linkedAtomsVector,
			std::vector<CycleState> & p_cycleStatePerAtoms
		)
		{
			std::vector<Index> cycleIndexes;
			cycleIndexes.resize( 6, 0 );

			p_cycleStatePerAtoms.clear();
			p_cycleStatePerAtoms.resize( p_topology.getAtomCount(), CycleState::None );

			for ( Index atomIndex = 0; atomIndex < p_topology.getAtomCount(); atomIndex++ )
			{
				cycleIndexes[ 0 ] = atomIndex;
				_tagCyclesRecursive( p_topology, p_frame, p_linkedAtomsVector, p_cycleStatePerAtoms, cycleIndexes, 1 );
			}
		}

		// Set a bond order from an atom local adjacency list.
		void _setBondOrder(
			Topology &						p_topology,
			const std::vector<LinkedAtom> & p_linkedAtoms,
			size_t &						p_recomputedBondOrderCount,
			const Index						p_secondAtomIndex,
			const ORDER						p_bondOrder,
			const bool						p_force = false
		)
		{
			for ( const LinkedAtom & linkedAtom : p_linkedAtoms )
			{
				if ( linkedAtom.atomIndex != p_secondAtomIndex )
				{
					continue;
				}

				ORDER & bondOrder = p_topology.bondOrders[ linkedAtom.bondIndex ];
				if ( ( p_force || bondOrder == ORDER::UNKNOWN ) && bondOrder != p_bondOrder )
				{
					bondOrder = p_bondOrder;
					p_recomputedBondOrderCount++;
				}

				return;
			}
		}

		// Apply legacy geometric heuristics to unknown bond orders.
		void _checkBondOrders(
			Topology &						p_topology,
			const Frame &					p_frame,
			const NeighbourIndexes &		p_linkedAtomsVector,
			const std::vector<CycleState> & p_cycleStatePerAtom,
			size_t &						p_recomputedBondOrderCount
		)
		{
			const float planarCutoff = 0.96f;

			for ( Index atomIndex = 0; atomIndex < p_topology.getAtomCount(); atomIndex++ )
			{
				const SYMBOL atomSymbol = p_topology.getAtomSymbol( atomIndex );
				if ( atomSymbol != SYMBOL::A_P && atomSymbol != SYMBOL::A_S && atomSymbol != SYMBOL::A_N
					 && atomSymbol != SYMBOL::A_C )
				{
					continue;
				}

				const std::vector<LinkedAtom> & linkedAtoms	 = p_linkedAtomsVector[ atomIndex ];
				const Vec3f &					atomPosition = p_frame[ atomIndex ];
				std::vector<Index>				linkedAtomIndexes;
				linkedAtomIndexes.reserve( linkedAtoms.size() );

				NeighbourData neighbourData;
				const size_t  neighbourCount = linkedAtoms.size();

				for ( const LinkedAtom & nextAtom : linkedAtoms )
				{
					const Index nextAtomIndex = nextAtom.atomIndex;
					linkedAtomIndexes.emplace_back( nextAtomIndex );

					const Vec3f & nextAtomPosition = p_frame[ nextAtomIndex ];
					const float	  atomDistance	   = length( nextAtomPosition - atomPosition );

					AtomData atomData { nextAtomIndex, nextAtomPosition, atomDistance };

					const SYMBOL nextAtomSymbol = p_topology.getAtomSymbol( nextAtomIndex );
					if ( nextAtomSymbol == SYMBOL::A_C )
					{
						neighbourData.carbons.emplace_back( atomData );
					}
					else if ( nextAtomSymbol == SYMBOL::A_O )
					{
						neighbourData.oxygens.emplace_back( atomData );
					}
					else if ( nextAtomSymbol == SYMBOL::A_N )
					{
						neighbourData.nitrogens.emplace_back( atomData );
					}
				}

				if ( atomSymbol == SYMBOL::A_C )
				{
					const float avgDotCross = _computeAverageCenterDotCross( p_frame, linkedAtomIndexes );

					if ( neighbourCount == 3 && avgDotCross > planarCutoff )
					{
						const size_t oxygenCountInNeighbours   = neighbourData.oxygens.size();
						const size_t nitrogenCountInNeighbours = neighbourData.nitrogens.size();

						if ( nitrogenCountInNeighbours >= 1 && oxygenCountInNeighbours == 1 )
						{
							const Index firstOxygenIndex = neighbourData.oxygens[ 0 ].index;

							if ( neighbourData.oxygens[ 0 ].distance < 1.38f
								 && p_linkedAtomsVector[ firstOxygenIndex ].size() == 1 )
							{
								_setBondOrder(
									p_topology, linkedAtoms, p_recomputedBondOrderCount, firstOxygenIndex, ORDER::DOUBLE
								);
							}
						}
						else if ( nitrogenCountInNeighbours >= 1 && oxygenCountInNeighbours >= 2 )
						{
							const Index firstOxygenIndex  = neighbourData.oxygens[ 0 ].index;
							const Index secondOxygenIndex = neighbourData.oxygens[ 1 ].index;

							const bool firstOxygenIsValid  = neighbourData.oxygens[ 0 ].distance < 1.38f
															 && p_linkedAtomsVector[ firstOxygenIndex ].size() == 1;
							const bool secondOxygenIsValid = neighbourData.oxygens[ 1 ].distance < 1.38f
															 && p_linkedAtomsVector[ secondOxygenIndex ].size() == 1;

							if ( firstOxygenIsValid && secondOxygenIsValid )
							{
								_setBondOrder(
									p_topology, linkedAtoms, p_recomputedBondOrderCount, firstOxygenIndex, ORDER::DOUBLE
								);
								_setBondOrder(
									p_topology,
									linkedAtoms,
									p_recomputedBondOrderCount,
									secondOxygenIndex,
									ORDER::SINGLE
								);
							}
							else if ( firstOxygenIsValid )
							{
								_setBondOrder(
									p_topology, linkedAtoms, p_recomputedBondOrderCount, firstOxygenIndex, ORDER::DOUBLE
								);
							}
							else if ( secondOxygenIsValid )
							{
								_setBondOrder(
									p_topology,
									linkedAtoms,
									p_recomputedBondOrderCount,
									secondOxygenIndex,
									ORDER::DOUBLE
								);
							}
						}
						else if ( nitrogenCountInNeighbours == 0 && oxygenCountInNeighbours == 1 )
						{
							const Index firstOxygenIndex = neighbourData.oxygens[ 0 ].index;

							if ( neighbourData.oxygens[ 0 ].distance < 1.31f
								 && p_linkedAtomsVector[ firstOxygenIndex ].size() == 1 )
							{
								_setBondOrder(
									p_topology, linkedAtoms, p_recomputedBondOrderCount, firstOxygenIndex, ORDER::DOUBLE
								);
							}
						}
						else if ( nitrogenCountInNeighbours == 0 && oxygenCountInNeighbours >= 2 )
						{
							const Index firstOxygenIndex  = neighbourData.oxygens[ 0 ].index;
							const Index secondOxygenIndex = neighbourData.oxygens[ 1 ].index;

							const bool firstOxygenIsValid  = neighbourData.oxygens[ 0 ].distance < 1.38f
															 && p_linkedAtomsVector[ firstOxygenIndex ].size() == 1;
							const bool secondOxygenIsValid = neighbourData.oxygens[ 1 ].distance < 1.38f
															 && p_linkedAtomsVector[ secondOxygenIndex ].size() == 1;

							if ( firstOxygenIsValid && secondOxygenIsValid )
							{
								_setBondOrder(
									p_topology, linkedAtoms, p_recomputedBondOrderCount, firstOxygenIndex, ORDER::DOUBLE
								);
								_setBondOrder(
									p_topology,
									linkedAtoms,
									p_recomputedBondOrderCount,
									secondOxygenIndex,
									ORDER::SINGLE
								);
							}
							else if ( firstOxygenIsValid )
							{
								_setBondOrder(
									p_topology, linkedAtoms, p_recomputedBondOrderCount, firstOxygenIndex, ORDER::DOUBLE
								);
							}
							else if ( secondOxygenIsValid )
							{
								_setBondOrder(
									p_topology,
									linkedAtoms,
									p_recomputedBondOrderCount,
									secondOxygenIndex,
									ORDER::DOUBLE
								);
							}
						}
						else if ( nitrogenCountInNeighbours >= 3 )
						{
							if ( neighbourData.nitrogens[ 0 ].distance < 1.44f
								 && neighbourData.nitrogens[ 1 ].distance < 1.44f
								 && neighbourData.nitrogens[ 2 ].distance < 1.44f )
							{
								const Index n0Index = neighbourData.nitrogens[ 0 ].index;
								const Index n1Index = neighbourData.nitrogens[ 1 ].index;
								const Index n2Index = neighbourData.nitrogens[ 2 ].index;

								const size_t n0NeighbourCount = p_linkedAtomsVector[ n0Index ].size();
								const size_t n1NeighbourCount = p_linkedAtomsVector[ n1Index ].size();
								const size_t n2NeighbourCount = p_linkedAtomsVector[ n2Index ].size();

								if ( n0NeighbourCount == 1 && n1NeighbourCount == 1 && n2NeighbourCount >= 2 )
								{
									_setBondOrder(
										p_topology, linkedAtoms, p_recomputedBondOrderCount, n0Index, ORDER::SINGLE
									);
									_setBondOrder(
										p_topology, linkedAtoms, p_recomputedBondOrderCount, n1Index, ORDER::SINGLE
									);
								}
								else if ( n0NeighbourCount == 1 && n1NeighbourCount >= 2 && n2NeighbourCount == 1 )
								{
									_setBondOrder(
										p_topology, linkedAtoms, p_recomputedBondOrderCount, n0Index, ORDER::SINGLE
									);
									_setBondOrder(
										p_topology, linkedAtoms, p_recomputedBondOrderCount, n2Index, ORDER::SINGLE
									);
								}
								else if ( n0NeighbourCount >= 2 && n1NeighbourCount == 1 && n2NeighbourCount == 1 )
								{
									_setBondOrder(
										p_topology, linkedAtoms, p_recomputedBondOrderCount, n1Index, ORDER::SINGLE
									);
									_setBondOrder(
										p_topology, linkedAtoms, p_recomputedBondOrderCount, n2Index, ORDER::DOUBLE
									);
								}
							}
						}
						else if ( nitrogenCountInNeighbours == 2 && not neighbourData.carbons.empty() )
						{
							if ( neighbourData.nitrogens[ 0 ].distance < 1.43f
								 && neighbourData.nitrogens[ 1 ].distance < 1.43f )
							{
								const Index n0Index = neighbourData.nitrogens[ 0 ].index;
								const Index n1Index = neighbourData.nitrogens[ 1 ].index;

								if ( p_cycleStatePerAtom[ neighbourData.carbons[ 0 ].index ] == CycleState::Planar
									 && p_cycleStatePerAtom[ atomIndex ] == CycleState::None
									 && p_cycleStatePerAtom[ n0Index ] == CycleState::None
									 && p_cycleStatePerAtom[ n1Index ] == CycleState::None )
								{
									_setBondOrder(
										p_topology, linkedAtoms, p_recomputedBondOrderCount, n0Index, ORDER::SINGLE
									);
									_setBondOrder(
										p_topology, linkedAtoms, p_recomputedBondOrderCount, n1Index, ORDER::SINGLE
									);
								}
							}
						}
					}

					if ( ( neighbourCount == 2 || ( neighbourCount == 3 && avgDotCross > planarCutoff ) )
						 && neighbourData.nitrogens.size() == 1 && neighbourData.oxygens.empty()
						 && neighbourData.nitrogens[ 0 ].distance < 1.36f
						 && p_cycleStatePerAtom[ neighbourData.nitrogens[ 0 ].index ] == CycleState::None
						 && p_cycleStatePerAtom[ atomIndex ] == CycleState::None )
					{
						const Index n0Index	   = neighbourData.nitrogens[ 0 ].index;
						float		n0DotCross = 1.f;
						if ( p_linkedAtomsVector[ n0Index ].size() == 3 )
						{
							std::vector<Index> atomsForPlanarity;
							atomsForPlanarity.resize( 4 );
							atomsForPlanarity[ 0 ] = n0Index;

							for ( size_t i = 0; i < 3; i++ )
							{
								atomsForPlanarity[ i + 1 ] = p_linkedAtomsVector[ n0Index ][ i ].atomIndex;
							}

							n0DotCross = _computeAverageCenterDotCross( p_frame, atomsForPlanarity );
						}

						if ( n0DotCross > planarCutoff )
						{
							_setBondOrder(
								p_topology, linkedAtoms, p_recomputedBondOrderCount, n0Index, ORDER::DOUBLE
							);

							if ( neighbourData.nitrogens[ 0 ].distance < 1.24f && not neighbourData.carbons.empty()
								 && p_linkedAtomsVector[ n0Index ].size() == 1 )
							{
								const Vec3f n0Vector
									= safeNormalize( neighbourData.nitrogens[ 0 ].position - atomPosition );
								const Vec3f c0Vector
									= safeNormalize( neighbourData.carbons[ 0 ].position - atomPosition );

								if ( dot( n0Vector, c0Vector ) < -0.9f )
								{
									_setBondOrder(
										p_topology, linkedAtoms, p_recomputedBondOrderCount, n0Index, ORDER::TRIPLE
									);
								}
							}
						}
					}
				}
				else if ( atomSymbol == SYMBOL::A_N )
				{
					if ( neighbourCount == 3 && neighbourData.oxygens.size() == 2
						 && _computeAverageCenterDotCross( p_frame, linkedAtomIndexes ) > planarCutoff )
					{
						const Index firstOxygenIndex = neighbourData.oxygens[ 0 ].index;
						if ( p_linkedAtomsVector[ firstOxygenIndex ].size() == 1 )
						{
							_setBondOrder(
								p_topology, linkedAtoms, p_recomputedBondOrderCount, firstOxygenIndex, ORDER::SINGLE
							);
						}

						const Index secondOxygenIndex = neighbourData.oxygens[ 1 ].index;
						if ( p_linkedAtomsVector[ secondOxygenIndex ].size() == 1 )
						{
							_setBondOrder(
								p_topology, linkedAtoms, p_recomputedBondOrderCount, secondOxygenIndex, ORDER::SINGLE
							);
						}
					}
				}
				else if ( atomSymbol == SYMBOL::A_S || atomSymbol == SYMBOL::A_P )
				{
					if ( neighbourData.oxygens.size() >= 4 )
					{
						for ( const AtomData & oxygen : neighbourData.oxygens )
						{
							if ( p_linkedAtomsVector[ oxygen.index ].size() == 1 )
							{
								_setBondOrder(
									p_topology, linkedAtoms, p_recomputedBondOrderCount, oxygen.index, ORDER::DOUBLE
								);
								break;
							}
						}
					}
					else if ( neighbourData.oxygens.size() >= 2 )
					{
						for ( const AtomData & oxygen : neighbourData.oxygens )
						{
							if ( p_linkedAtomsVector[ oxygen.index ].size() == 1 )
							{
								_setBondOrder(
									p_topology, linkedAtoms, p_recomputedBondOrderCount, oxygen.index, ORDER::DOUBLE
								);
							}
						}
					}
				}

				if ( p_cycleStatePerAtom[ atomIndex ] == CycleState::Planar && _isConjugableRingAtom( atomSymbol ) )
				{
					for ( const LinkedAtom & neighbour : linkedAtoms )
					{
						const Index neighbourIndex = neighbour.atomIndex;
						if ( p_cycleStatePerAtom[ neighbourIndex ] == CycleState::Planar
							 && _isConjugableRingAtom( p_topology.getAtomSymbol( neighbourIndex ) ) )
						{
							_setBondOrder(
								p_topology, linkedAtoms, p_recomputedBondOrderCount, neighbourIndex, ORDER::AROMATIC
							);
						}
					}
				}
			}
		}
	} // namespace

	void recomputeBondOrders( VTX::Core::Struct::Topology & p_topology, const VTX::Core::Struct::Frame & p_frame )
	{
		VTX::Util::ScopedChrono chrono( "BondOrderGuessing::recomputeBondOrders" );
		VTX_INFO( "Guessing bond orders..." );

		if ( p_frame.size() != p_topology.getAtomCount() )
		{
			VTX_INFO( "Recomputed 0 bond orders" );
			return;
		}

		NeighbourIndexes linkedAtomsVector;
		const bool		 hasUnknownBondOrders = _buildNeighbourStruct( p_topology, linkedAtomsVector );

		if ( not hasUnknownBondOrders )
		{
			VTX_INFO( "Recomputed 0 bond orders" );
			return;
		}

		std::vector<CycleState> cycleStatePerAtom;
		_tagCycles( p_topology, p_frame, linkedAtomsVector, cycleStatePerAtom );

		size_t recomputedBondOrderCount = 0;
		_checkBondOrders( p_topology, p_frame, linkedAtomsVector, cycleStatePerAtom, recomputedBondOrderCount );

		VTX_INFO( "Recomputed {} bond orders", recomputedBondOrderCount );
	}

} // namespace VTX::IO::Util
