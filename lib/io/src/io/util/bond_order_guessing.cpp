#include "io/util/bond_order_guessing.hpp"
#include "io/util/chemfiles.hpp"
#include <optional>
#include <sstream>

namespace VTX::IO::Util
{
	namespace ChemDB = VTX::Core::ChemDB;

	void BondOrderGuessing::recomputeBondOrders( chemfiles::Frame & p_frame )
	{
		std::vector<std::vector<size_t>> linkedAtomsVector = std::vector<std::vector<size_t>>();
		_buildNeighbourStruct( p_frame, linkedAtomsVector );

		std::vector<CycleState> cycleStatePerAtom = std::vector<CycleState>();
		_tagCycles( p_frame, linkedAtomsVector, cycleStatePerAtom );

		_checkBondOrders( p_frame, linkedAtomsVector, cycleStatePerAtom );
	}

	void BondOrderGuessing::_buildNeighbourStruct( const chemfiles::Frame &			  p_frame,
												   std::vector<std::vector<size_t>> & p_linkedAtomsVector )
	{
		std::vector<size_t> defaultBondVector = std::vector<size_t>();
		defaultBondVector.reserve( 4 );
		p_linkedAtomsVector.resize( p_frame.size(), defaultBondVector );

		for ( const chemfiles::Bond & bond : p_frame.topology().bonds() )
		{
			if ( p_frame[ bond[ 0 ] ].type() != "H" && p_frame[ bond[ 1 ] ].type() != "H" )
			{
				p_linkedAtomsVector[ bond[ 0 ] ].emplace_back( bond[ 1 ] );
				p_linkedAtomsVector[ bond[ 1 ] ].emplace_back( bond[ 0 ] );
			}
		}
	}

	void BondOrderGuessing::_tagCycles( const chemfiles::Frame &				 p_frame,
										const std::vector<std::vector<size_t>> & p_linkedAtomsVector,
										std::vector<CycleState> &				 p_cycleStatePerAtoms )
	{
		std::vector<size_t> cycleIndexes = std::vector<size_t>();
		cycleIndexes.resize( 6, -1 );

		p_cycleStatePerAtoms = std::vector<CycleState>();
		p_cycleStatePerAtoms.resize( p_frame.size(), CycleState::None );

		for ( size_t atom1 = 0; atom1 < p_frame.size(); atom1++ )
		{
			cycleIndexes[ 0 ] = atom1;
			_tagCyclesRecursive( p_frame, p_linkedAtomsVector, p_cycleStatePerAtoms, cycleIndexes, 1 );
		}
	}
	void BondOrderGuessing::_tagCyclesRecursive( const chemfiles::Frame &				  p_frame,
												 const std::vector<std::vector<size_t>> & p_linkedAtomsVector,
												 std::vector<CycleState> &				  p_cycleStatePerAtoms,
												 std::vector<size_t> &					  p_cycleIndexes,
												 short									  p_counter )
	{
		const size_t & currentAtomIndex = p_cycleIndexes[ p_counter - 1 ];
		for ( const size_t & nextAtomIndex : p_linkedAtomsVector[ currentAtomIndex ] )
		{
			bool nextAtomAlreadyInCycle = false;
			for ( int i = 0; i < p_counter - 1; i++ )
			{
				nextAtomAlreadyInCycle = nextAtomAlreadyInCycle || p_cycleIndexes[ i ] == nextAtomIndex;
			}

			if ( nextAtomAlreadyInCycle )
			{
				if ( p_counter >= 5 && nextAtomIndex == p_cycleIndexes[ 0 ] )
				{
					chemfiles::Vector3D dir;
					const float avgDotCross = _computeAverageRingDotCross( p_frame, p_cycleIndexes, p_counter, dir );
					const bool	isPlanar
						= avgDotCross > 0.96f
						  && _verifyPlanarBonds( p_frame, p_cycleIndexes, p_counter, p_linkedAtomsVector, dir, 0.35f );

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
					p_frame, p_linkedAtomsVector, p_cycleStatePerAtoms, p_cycleIndexes, p_counter + 1 );
			}
		}
	}

	void BondOrderGuessing::_checkBondOrders( chemfiles::Frame &					   p_frame,
											  const std::vector<std::vector<size_t>> & p_linkedAtomsVector,
											  const std::vector<CycleState> &		   p_cycleStatePerAtom )
	{
		const float planerCutoff = 0.96f;

		for ( size_t atomIndex = 0; atomIndex < p_frame.size(); atomIndex++ )
		{
			chemfiles::Atom & atom	   = p_frame[ atomIndex ];
			const std::string atomType = atom.type();
			if ( atomType == "P" || atomType == "S" || atomType == "N" || atomType == "C" )
			{
				const std::vector<size_t> & linkedAtoms	 = p_linkedAtomsVector[ atomIndex ];
				const chemfiles::Vector3D & atomIndexPos = p_frame.positions()[ atomIndex ];

				NeighbourData neighbourData	 = NeighbourData();
				const size_t  neighbourCount = linkedAtoms.size();

				for ( const size_t nextAtomIndex : linkedAtoms )
				{
					const chemfiles::Vector3D & nextAtomIndexPos = p_frame.positions()[ nextAtomIndex ];
					const float					atomDistance	 = float( ( nextAtomIndexPos - atomIndexPos ).norm() );

					NeighbourData::AtomData atomData
						= NeighbourData::AtomData( nextAtomIndex, nextAtomIndexPos, atomDistance );

					const std::string nextAtomType = p_frame[ nextAtomIndex ].type();

					if ( nextAtomType == "C" )
						neighbourData.carbons.emplace_back( atomData );
					else if ( nextAtomType == "O" )
						neighbourData.oxygens.emplace_back( atomData );
					else if ( nextAtomType == "N" )
						neighbourData.nitrogens.emplace_back( atomData );
				}

				if ( atomType == "C" )
				{
					const float avgDotCross = _computeAverageCenterDotCross( p_frame, linkedAtoms );

					// Planar carbons
					if ( neighbourCount == 3 )
					{
						if ( avgDotCross > planerCutoff )
						{
							const size_t oxygenCountInNeighbours   = neighbourData.oxygens.size();
							const size_t nitrogenCountInNeighbours = neighbourData.nitrogens.size();
							if ( nitrogenCountInNeighbours >= 1 && oxygenCountInNeighbours == 1 )
							{
								// Simple amide ?
								const size_t firstOxygenIndex = neighbourData.oxygens[ 0 ].getIndex();

								if ( neighbourData.oxygens[ 0 ].getDistance() < 1.38f
									 && p_linkedAtomsVector[ firstOxygenIndex ].size() == 1 )
								{
									_setBondOrder(
										p_frame, atomIndex, firstOxygenIndex, chemfiles::Bond::BondOrder::DOUBLE );
								}
							}
							else if ( nitrogenCountInNeighbours >= 1 && oxygenCountInNeighbours >= 2 )
							{
								// Carbamyl
								const size_t firstOxygenIndex  = neighbourData.oxygens[ 0 ].getIndex();
								const size_t secondOxygenIndex = neighbourData.oxygens[ 1 ].getIndex();

								const bool firstOxygenIsValid = neighbourData.oxygens[ 0 ].getDistance() < 1.38f
																&& p_linkedAtomsVector[ firstOxygenIndex ].size() == 1;
								const bool secondOxygenIsValid = neighbourData.oxygens[ 0 ].getDistance() < 1.38f
																 && p_linkedAtomsVector[ firstOxygenIndex ].size() == 1;

								if ( firstOxygenIsValid && secondOxygenIsValid )
								{
									// order = 4 in pymol
									_setBondOrder(
										p_frame, atomIndex, firstOxygenIndex, chemfiles::Bond::BondOrder::DOUBLE );
									// order = 4 in pymol
									_setBondOrder(
										p_frame, atomIndex, secondOxygenIndex, chemfiles::Bond::BondOrder::SINGLE );
								}
								else if ( firstOxygenIsValid )
								{
									_setBondOrder(
										p_frame, atomIndex, firstOxygenIndex, chemfiles::Bond::BondOrder::DOUBLE );
								}
								else if ( secondOxygenIsValid )
								{
									_setBondOrder(
										p_frame, atomIndex, secondOxygenIndex, chemfiles::Bond::BondOrder::DOUBLE );
								}
							}
							else if ( nitrogenCountInNeighbours == 0 && oxygenCountInNeighbours == 1 )
							{
								// Ketone
								const size_t firstOxygenIndex = neighbourData.oxygens[ 0 ].getIndex();

								if ( neighbourData.oxygens[ 0 ].getDistance() < 1.31f
									 && p_linkedAtomsVector[ firstOxygenIndex ].size() == 1 )
								{
									_setBondOrder(
										p_frame, atomIndex, firstOxygenIndex, chemfiles::Bond::BondOrder::DOUBLE );
								}
							}
							else if ( nitrogenCountInNeighbours == 0 && oxygenCountInNeighbours >= 2 )
							{
								// Simple Carboxylate ?
								const size_t firstOxygenIndex  = neighbourData.oxygens[ 0 ].getIndex();
								const size_t secondOxygenIndex = neighbourData.oxygens[ 1 ].getIndex();

								const bool firstOxygenIsValid = neighbourData.oxygens[ 0 ].getDistance() < 1.38f
																&& p_linkedAtomsVector[ firstOxygenIndex ].size() == 1;
								const bool secondOxygenIsValid = neighbourData.oxygens[ 0 ].getDistance() < 1.38f
																 && p_linkedAtomsVector[ firstOxygenIndex ].size() == 1;

								if ( firstOxygenIsValid && secondOxygenIsValid )
								{
									// ASP | GLU
									// order = 4 in pymol
									_setBondOrder(
										p_frame, atomIndex, firstOxygenIndex, chemfiles::Bond::BondOrder::DOUBLE );
									// order = 4 in pymol
									_setBondOrder(
										p_frame, atomIndex, secondOxygenIndex, chemfiles::Bond::BondOrder::SINGLE );
								}
								else if ( firstOxygenIsValid )
								{
									// Esther
									_setBondOrder(
										p_frame, atomIndex, firstOxygenIndex, chemfiles::Bond::BondOrder::DOUBLE );
								}
								else if ( secondOxygenIsValid )
								{
									_setBondOrder(
										p_frame, atomIndex, secondOxygenIndex, chemfiles::Bond::BondOrder::DOUBLE );
								}
							}
							else if ( nitrogenCountInNeighbours >= 3 )
							{
								// Guanido with no hydrogen
								if ( neighbourData.nitrogens[ 0 ].getDistance() < 1.44f
									 && neighbourData.nitrogens[ 1 ].getDistance() < 1.44f
									 && neighbourData.nitrogens[ 2 ].getDistance() < 1.44f )
								{
									const size_t n0Index = neighbourData.nitrogens[ 0 ].getIndex();
									const size_t n1Index = neighbourData.nitrogens[ 1 ].getIndex();
									const size_t n2Index = neighbourData.nitrogens[ 2 ].getIndex();

									const size_t n0NeighbourCount = p_linkedAtomsVector[ n0Index ].size();
									const size_t n1NeighbourCount = p_linkedAtomsVector[ n1Index ].size();
									const size_t n2NeighbourCount = p_linkedAtomsVector[ n2Index ].size();

									if ( n0NeighbourCount == 1 && n1NeighbourCount == 1 && n2NeighbourCount >= 2 )
									{
										// order = 4 in pymol
										_setBondOrder(
											p_frame, atomIndex, n0Index, chemfiles::Bond::BondOrder::SINGLE );
										// order = 4 in pymol
										_setBondOrder(
											p_frame, atomIndex, n1Index, chemfiles::Bond::BondOrder::SINGLE );
									}
									else if ( n0NeighbourCount == 1 && n1NeighbourCount >= 2 && n2NeighbourCount == 1 )
									{
										// order = 4 in pymol
										_setBondOrder(
											p_frame, atomIndex, n0Index, chemfiles::Bond::BondOrder::SINGLE );
										// order = 4 in pymol
										_setBondOrder(
											p_frame, atomIndex, n2Index, chemfiles::Bond::BondOrder::SINGLE );
									}
									else if ( n0NeighbourCount >= 2 && n1NeighbourCount == 1 && n2NeighbourCount == 1 )
									{
										// ARG
										// order = 4 in pymol
										_setBondOrder(
											p_frame, atomIndex, n1Index, chemfiles::Bond::BondOrder::SINGLE );
										// order = 4 in pymol
										_setBondOrder(
											p_frame, atomIndex, n2Index, chemfiles::Bond::BondOrder::DOUBLE );
									}
								}
							}
							else if ( nitrogenCountInNeighbours == 2 && neighbourData.carbons.size() > 0 )
							{
								if ( neighbourData.nitrogens[ 0 ].getDistance() < 1.43f
									 && neighbourData.nitrogens[ 1 ].getDistance() < 1.43f )
								{
									const size_t n0Index = neighbourData.nitrogens[ 0 ].getIndex();
									const size_t n1Index = neighbourData.nitrogens[ 1 ].getIndex();

									if ( p_cycleStatePerAtom[ neighbourData.carbons[ 0 ].getIndex() ]
											 == CycleState::Planar
										 && p_cycleStatePerAtom[ atomIndex ] == CycleState::None
										 && p_cycleStatePerAtom[ n0Index ] == CycleState::None
										 && p_cycleStatePerAtom[ n1Index ] == CycleState::None )
									{
										// order = 4 in pymol
										_setBondOrder(
											p_frame, atomIndex, n0Index, chemfiles::Bond::BondOrder::SINGLE );
										// order = 4 in pymol
										_setBondOrder(
											p_frame, atomIndex, n1Index, chemfiles::Bond::BondOrder::SINGLE );
									}
								}
							}
						}
					}

					// Any carbons
					// Imines and Nitriles
					if ( ( neighbourCount == 2 || ( neighbourCount == 3 && avgDotCross > planerCutoff ) )
						 && neighbourData.nitrogens.size() == 1 && neighbourData.oxygens.size() == 0
						 && neighbourData.nitrogens[ 0 ].getDistance() < 1.36f
						 && p_cycleStatePerAtom[ neighbourData.nitrogens[ 0 ].getIndex() ] == CycleState::None
						 && p_cycleStatePerAtom[ atomIndex ] == CycleState::None )
					{
						const size_t n0Index	= neighbourData.nitrogens[ 0 ].getIndex();
						float		 n0DotCross = 1.0f;
						if ( p_linkedAtomsVector[ n0Index ].size() == 3 )
						{
							std::vector<size_t> atomsForPlanarity = std::vector<size_t>();
							atomsForPlanarity.resize( 4 );
							atomsForPlanarity[ 0 ] = n0Index;

							for ( size_t i = 0; i < 3; i++ )
							{
								atomsForPlanarity[ i + 1 ] = p_linkedAtomsVector[ n0Index ][ i ];
							}

							n0DotCross = _computeAverageCenterDotCross( p_frame, atomsForPlanarity );
						}

						if ( n0DotCross > planerCutoff )
						{
							_setBondOrder( p_frame, atomIndex, n0Index, chemfiles::Bond::BondOrder::DOUBLE );
							if ( neighbourData.nitrogens[ 0 ].getDistance() < 1.24f && neighbourData.carbons.size() > 0
								 && p_linkedAtomsVector[ n0Index ].size() == 1 )
							{
								chemfiles::Vector3D n0Vector
									= neighbourData.nitrogens[ 0 ].getChemfilesPosition() - atomIndexPos;
								_normalizeVector( n0Vector );
								chemfiles::Vector3D c0Vector
									= neighbourData.carbons[ 0 ].getChemfilesPosition() - atomIndexPos;
								_normalizeVector( c0Vector );

								if ( chemfiles::dot( n0Vector, c0Vector ) < -0.9 )
								{
									_setBondOrder( p_frame, atomIndex, n0Index, chemfiles::Bond::BondOrder::TRIPLE );
								}
							}
						}
					}
				}
				else if ( atomType == "N" )
				{
					if ( neighbourCount == 3 && neighbourData.oxygens.size() == 2
						 && _computeAverageCenterDotCross( p_frame, linkedAtoms ) > planerCutoff )
					{
						const size_t firstOxygenIndex = neighbourData.oxygens[ 0 ].getIndex();
						if ( p_linkedAtomsVector[ firstOxygenIndex ].size() == 1 )
						{
							// order = 4 in pymol
							_setBondOrder( p_frame, atomIndex, firstOxygenIndex, chemfiles::Bond::BondOrder::SINGLE );
						}

						const size_t secondOxygenIndex = neighbourData.oxygens[ 1 ].getIndex();
						if ( p_linkedAtomsVector[ secondOxygenIndex ].size() == 1 )
						{
							// order = 4 in pymol
							_setBondOrder( p_frame, atomIndex, secondOxygenIndex, chemfiles::Bond::BondOrder::SINGLE );
						}
					}
				}
				else if ( atomType == "S" || atomType == "P" )
				{
					if ( neighbourData.oxygens.size() >= 4 )
					{
						for ( size_t i = 0; i < neighbourData.oxygens.size(); i++ )
						{
							const size_t oxygenIndex = neighbourData.oxygens[ i ].getIndex();
							if ( p_linkedAtomsVector[ oxygenIndex ].size() == 1 )
							{
								_setBondOrder( p_frame, atomIndex, oxygenIndex, chemfiles::Bond::BondOrder::DOUBLE );
								break;
							}
						}
					}
					else if ( neighbourData.oxygens.size() >= 2 )
					{
						for ( size_t i = 0; i < neighbourData.oxygens.size(); i++ )
						{
							const size_t oxygenIndex = neighbourData.oxygens[ i ].getIndex();
							if ( p_linkedAtomsVector[ oxygenIndex ].size() == 1 )
							{
								_setBondOrder( p_frame, atomIndex, oxygenIndex, chemfiles::Bond::BondOrder::DOUBLE );
							}
						}
					}
				}

				if ( p_cycleStatePerAtom[ atomIndex ] == CycleState::Planar )
				{
					if ( atomType == "C" || atomType == "N" || atomType == "O" || atomType == "S" )
					{
						for ( const size_t neighbourIndex : linkedAtoms )
						{
							if ( p_cycleStatePerAtom[ neighbourIndex ] == CycleState::Planar )
							{
								const std::string & neighbourType = p_frame[ neighbourIndex ].type();
								if ( neighbourType == "C" || neighbourType == "N" || neighbourType == "O"
									 || neighbourType == "S" )
								{
									// order = 4 in pymol
									_setBondOrder(
										p_frame, atomIndex, neighbourIndex, chemfiles::Bond::BondOrder::AROMATIC );
								}
							}
						}
					}
				}
			}
		}
	}

	void BondOrderGuessing::_normalizeVector( chemfiles::Vector3D & p_vector )
	{
		const double dist = p_vector.norm();
		p_vector[ 0 ] /= dist;
		p_vector[ 1 ] /= dist;
		p_vector[ 2 ] /= dist;
	}

	float BondOrderGuessing::_computeAverageCenterDotCross( const chemfiles::Frame &	p_frame,
															const std::vector<size_t> & p_atoms )
	{
		float		 result	   = 0.0F;
		const size_t atomCount = p_atoms.size();

		if ( atomCount == 0 )
			return result;

		std::vector<chemfiles::Vector3D> positions = std::vector<chemfiles::Vector3D>();
		positions.resize( atomCount + 1 );

		for ( size_t i = 0; i < atomCount; i++ )
			positions[ i ] = p_frame.positions()[ p_atoms[ i ] ];

		positions[ atomCount ] = positions[ 1 ];

		std::vector<chemfiles::Vector3D> crossProducts = std::vector<chemfiles::Vector3D>();
		crossProducts.resize( atomCount + 1 );

		for ( size_t i = 1; i < atomCount; i++ )
		{
			chemfiles::Vector3D vec0 = positions[ i ] - positions[ 0 ];
			_normalizeVector( vec0 );
			chemfiles::Vector3D vec1 = positions[ i + 1 ] - positions[ 0 ];
			_normalizeVector( vec1 );

			crossProducts[ i ] = chemfiles::cross( vec0, vec1 );
			_normalizeVector( crossProducts[ i ] );

			if ( i > 1 && chemfiles::dot( crossProducts[ i - 1 ], crossProducts[ i ] ) < 0.0 )
				crossProducts[ i ] = -crossProducts[ i ];
		}
		crossProducts[ atomCount ] = crossProducts[ 1 ];

		float avg = 0.f;
		for ( size_t i = 1; i < atomCount; i++ )
		{
			avg += float( chemfiles::dot( crossProducts[ i ], crossProducts[ i + 1 ] ) );
		}

		return avg / ( atomCount - 1 );
	}
	float BondOrderGuessing::_computeAverageRingDotCross( const chemfiles::Frame &	  p_frame,
														  const std::vector<size_t> & p_atoms,
														  const size_t				  p_atomCount,
														  chemfiles::Vector3D &		  dir )
	{
		float							 result	   = 0.f;
		std::vector<chemfiles::Vector3D> positions = std::vector<chemfiles::Vector3D>();
		positions.resize( p_atomCount + 2 );

		for ( size_t i = 0; i < p_atomCount; i++ )
			positions[ i ] = p_frame.positions()[ p_atoms[ i ] ];

		std::vector<chemfiles::Vector3D> crossProducts = std::vector<chemfiles::Vector3D>();
		crossProducts.resize( p_atomCount + 1 );

		float avg = 0.0;

		positions[ p_atomCount ]	 = positions[ 0 ];
		positions[ p_atomCount + 1 ] = positions[ 1 ];

		for ( size_t i = 0; i < p_atomCount; i++ )
		{
			chemfiles::Vector3D vec0 = positions[ i ] - positions[ i + 1 ];
			_normalizeVector( vec0 );
			chemfiles::Vector3D vec1 = positions[ i + 2 ] - positions[ i + 1 ];
			_normalizeVector( vec1 );

			crossProducts[ i ] = chemfiles::cross( vec0, vec1 );
			_normalizeVector( crossProducts[ i ] );

			if ( i > 1 && chemfiles::dot( crossProducts[ i - 1 ], crossProducts[ i ] ) < 0.0 )
				crossProducts[ i ] = -crossProducts[ i ];

			dir += crossProducts[ i ];
		}

		crossProducts[ p_atomCount ] = crossProducts[ 0 ];

		for ( size_t i = 0; i < p_atomCount; i++ )
		{
			avg += float( chemfiles::dot( crossProducts[ i ], crossProducts[ i + 1 ] ) );
		}
		result = avg / p_atomCount;

		return result;
	}
	bool BondOrderGuessing::_verifyPlanarBonds( const chemfiles::Frame &				 p_frame,
												const std::vector<size_t> &				 p_atoms,
												const size_t							 p_atomCount,
												const std::vector<std::vector<size_t>> & p_linkedAtomsVector,
												const chemfiles::Vector3D &				 dir,
												const float								 cutoff )
	{
		for ( size_t i = 0; i < p_atomCount; i++ )
		{
			const size_t atomIndex = p_atoms[ i ];

			const chemfiles::Vector3D & atomPos	 = p_frame.positions()[ atomIndex ];
			const std::string			atomType = p_frame[ atomIndex ].type();

			if ( !( atomType == "C" || atomType == "N" || atomType == "O" || atomType == "S" ) )
				continue;

			for ( const size_t neighbourIndex : p_linkedAtomsVector[ atomIndex ] )
			{
				const std::string neighbourType = p_frame[ neighbourIndex ].type();

				if ( !( atomType == "C" || atomType == "N" || atomType == "O" || atomType == "S" ) )
					continue;

				const chemfiles::Vector3D & neighbourPos = p_frame.positions()[ neighbourIndex ];
				chemfiles::Vector3D			vec			 = neighbourPos - atomPos;
				_normalizeVector( vec );
				float dot = float( chemfiles::dot( vec, dir ) );
				dot		  = dot < 0 ? -dot : dot;

				if ( dot > cutoff )
					return false;
			}
		}

		return true;
	}
	void BondOrderGuessing::_setBondOrder( chemfiles::Frame &				p_frame,
										   const size_t						p_firstAtomIndex,
										   const size_t						p_secondAtomIndex,
										   const chemfiles::Bond::BondOrder p_bondOrder,
										   const bool						p_force )
	{
		if ( p_force
			 || p_frame.topology().bond_order( p_firstAtomIndex, p_secondAtomIndex )
					== chemfiles::Bond::BondOrder::UNKNOWN )
		{
			p_frame.change_bond_order( p_firstAtomIndex, p_secondAtomIndex, p_bondOrder );
		}
	}

	bool BondOrderGuessing::recomputeBondOrdersFromFile( chemfiles::Frame & p_frame )
	{
		bool res = true;

		size_t					   bondDataCurrentIndex = 0;
		const chemfiles::Residue * previousResidue		= nullptr;

		for ( size_t iBond = 0; iBond < p_frame.topology().bonds().size(); iBond++ )
		{
			if ( p_frame.topology().bond_orders()[ iBond ] != chemfiles::Bond::BondOrder::UNKNOWN )
				continue;

			const chemfiles::Bond & bond = p_frame.topology().bonds()[ iBond ];

			const size_t firstAtomIndex	 = bond[ 0 ];
			const size_t secondAtomIndex = bond[ 1 ];

			const std::experimental::optional<const chemfiles::Residue &> firstResidue
				= p_frame.topology().residue_for_atom( firstAtomIndex );

			std::experimental::optional<const chemfiles::Residue &> secondResidue
				= p_frame.topology().residue_for_atom( secondAtomIndex );

			if ( !bool( firstResidue ) || !bool( secondResidue ) )
			{
				// Unknown residue => maybe recompute not complete
				res = false;
				continue;
			}
			else if ( firstResidue.value() != secondResidue.value() )
			{
				continue;
			}

			const chemfiles::Residue & residue = firstResidue.value();

			const std::vector<Struct::BondData> & bondsData = getResidueBonds( residue.name() );

			if ( previousResidue != &residue )
			{
				bondDataCurrentIndex = 0;
				previousResidue		 = &residue;
			}
			else
			{
				bondDataCurrentIndex++;
			}

			const std::string firstAtomName	 = p_frame[ firstAtomIndex ].name();
			const std::string secondAtomName = p_frame[ secondAtomIndex ].name();

			for ( ; bondDataCurrentIndex < bondsData.size(); bondDataCurrentIndex++ )
			{
				const Struct::BondData & bondData = bondsData[ bondDataCurrentIndex ];

				if ( bondData.atom1 == firstAtomName && bondData.atom2 == secondAtomName )
				{
					p_frame.change_bond_order(
						firstAtomIndex, secondAtomIndex, IO::Util::Chemfiles::convertBondOrder( bondData.bondOrder ) );
					break;
				}
			}
		}

		return res;
	}

	const std::vector<Struct::BondData> & BondOrderGuessing::getResidueBonds( const std::string & p_residueSymbol )
	{
		throw NotImplementedException();
		// return Util::App::Old::System::getResidueBonds( p_residueSymbol );
	}

} // namespace VTX::IO::Util
