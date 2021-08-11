#include "util/chemfiles.hpp"
#include "model/atom.hpp"
#include "model/residue.hpp"
#include <iostream>

namespace VTX::Util::Chemfiles
{
	void CellList::addAtom( const size_t p_atomIndex, const chemfiles::Vector3D & p_position )
	{
		const size_t atomIndex = _getIndexFromPosition( p_position );
		_cellList[ atomIndex ].emplace_back( p_atomIndex );
	}
	void CellList::addAtomFromNonStandardResidue( const size_t p_atomIndex, const chemfiles::Vector3D & p_position )
	{
		const size_t cellIndex = _getIndexFromPosition( p_position );
		_cellList[ cellIndex ].emplace_back( p_atomIndex );
		_nonStdAtoms[ cellIndex ].emplace_back( p_atomIndex );
	}

	void CellList::addCysteineSulfur( const size_t p_atomIndex, const chemfiles::Vector3D & p_position )
	{
		const size_t cellIndex = _getIndexFromPosition( p_position );
		_cellList[ cellIndex ].emplace_back( p_atomIndex );
		_cysteinSulfurIndexes[ cellIndex ].emplace_back( p_atomIndex );
	}

	const std::vector<size_t> & CellList::getNeighbours( const size_t & p_index ) const
	{
		return _neighbourList[ p_index ];
	}
	const std::vector<size_t> & CellList::getNeighbours( const chemfiles::Vector3D & p_position ) const
	{
		const size_t cellIndex = _getIndexFromPosition( p_position );
		return _neighbourList[ cellIndex ];
	}
	const std::vector<size_t> & CellList::getCysteineSulfurAtoms( const size_t p_cellIndex ) const
	{
		return _cysteinSulfurIndexes[ p_cellIndex ];
	}
	const std::vector<std::vector<size_t>> & CellList::getCysteineSulfurAtoms() const { return _cysteinSulfurIndexes; }
	const std::vector<std::vector<size_t>> & CellList::getNonStdAtoms() const { return _nonStdAtoms; }
	std::vector<std::vector<size_t>> &		 CellList::getNonStdAtoms() { return _nonStdAtoms; }

	size_t CellList::_getIndexFromPosition( const chemfiles::Vector3D & p_position ) const
	{
		const size_t xIndex = int( ( p_position[ 0 ] - _xStart ) / _cubeSize );
		const size_t yIndex = int( ( p_position[ 1 ] - _yStart ) / _cubeSize );
		const size_t zIndex = int( ( p_position[ 2 ] - _zStart ) / _cubeSize );

		return xIndex * _height * _depth + yIndex * _depth + zIndex;
	}

	void CellList::_generateNeighbourList()
	{
		_neighbourList = std::vector<std::vector<size_t>>();
		_neighbourList.resize( _cellList.size() );

		const size_t xOffset = _height * _depth;
		const size_t yOffset = _depth;
		const size_t zOffset = 1;

		for ( int i = 0; i < _width; i++ )
		{
			for ( int j = 0; j < _height; j++ )
			{
				for ( int k = 0; k < _depth; k++ )
				{
					const size_t index		= i * _height * _depth + j * _depth + k;
					_neighbourList[ index ] = std::vector<size_t>();
					_neighbourList[ index ].reserve( 27 );

					_neighbourList[ index ].emplace_back( index );

					const bool hasNeighbourAtLeft	  = i > 0;
					const bool hasNeighbourAtRight	  = i < _width - 1;
					const bool hasNeighbourAtBottom	  = j > 0;
					const bool hasNeighbourAtTop	  = j < _height - 1;
					const bool hasNeighbourAtBackward = k > 0;
					const bool hasNeighbourAtForward  = k < _depth - 1;

					if ( hasNeighbourAtLeft )
					{
						_neighbourList[ index ].emplace_back( index - xOffset );
						if ( hasNeighbourAtBottom )
						{
							_neighbourList[ index ].emplace_back( index - xOffset - yOffset );

							if ( hasNeighbourAtBackward )
							{
								_neighbourList[ index ].emplace_back( index - xOffset - yOffset - zOffset );
							}
							if ( hasNeighbourAtForward )
							{
								_neighbourList[ index ].emplace_back( index - xOffset - yOffset + zOffset );
							}
						}

						if ( hasNeighbourAtTop )
						{
							_neighbourList[ index ].emplace_back( index - xOffset + yOffset );

							if ( hasNeighbourAtBackward )
							{
								_neighbourList[ index ].emplace_back( index - xOffset + yOffset - zOffset );
							}
							if ( hasNeighbourAtForward )
							{
								_neighbourList[ index ].emplace_back( index - xOffset + yOffset + zOffset );
							}
						}

						if ( hasNeighbourAtBackward )
						{
							_neighbourList[ index ].emplace_back( index - xOffset - zOffset );
						}
						if ( hasNeighbourAtForward )
						{
							_neighbourList[ index ].emplace_back( index - xOffset + zOffset );
						}
					}
					if ( hasNeighbourAtRight )
					{
						_neighbourList[ index ].emplace_back( index + xOffset );
						if ( hasNeighbourAtBottom )
						{
							_neighbourList[ index ].emplace_back( index + xOffset - yOffset );

							if ( hasNeighbourAtBackward )
							{
								_neighbourList[ index ].emplace_back( index + xOffset - yOffset - zOffset );
							}
							if ( hasNeighbourAtForward )
							{
								_neighbourList[ index ].emplace_back( index + xOffset - yOffset + zOffset );
							}
						}

						if ( hasNeighbourAtTop )
						{
							_neighbourList[ index ].emplace_back( index + xOffset + yOffset );

							if ( hasNeighbourAtBackward )
							{
								_neighbourList[ index ].emplace_back( index + xOffset + yOffset - zOffset );
							}
							if ( hasNeighbourAtForward )
							{
								_neighbourList[ index ].emplace_back( index + xOffset + yOffset + zOffset );
							}
						}

						if ( hasNeighbourAtBackward )
						{
							_neighbourList[ index ].emplace_back( index + xOffset - zOffset );
						}
						if ( hasNeighbourAtForward )
						{
							_neighbourList[ index ].emplace_back( index + xOffset + zOffset );
						}
					}

					if ( hasNeighbourAtBottom )
					{
						_neighbourList[ index ].emplace_back( index - yOffset );

						if ( hasNeighbourAtBackward )
						{
							_neighbourList[ index ].emplace_back( index - yOffset - zOffset );
						}
						if ( hasNeighbourAtForward )
						{
							_neighbourList[ index ].emplace_back( index - yOffset + zOffset );
						}
					}
					if ( hasNeighbourAtTop )
					{
						_neighbourList[ index ].emplace_back( index + yOffset );

						if ( hasNeighbourAtBackward )
						{
							_neighbourList[ index ].emplace_back( index + yOffset - zOffset );
						}
						if ( hasNeighbourAtForward )
						{
							_neighbourList[ index ].emplace_back( index + yOffset + zOffset );
						}
					}

					if ( hasNeighbourAtBackward )
					{
						_neighbourList[ index ].emplace_back( index - zOffset );
					}
					if ( hasNeighbourAtForward )
					{
						_neighbourList[ index ].emplace_back( index + zOffset );
					}

					_neighbourList[ index ].shrink_to_fit();
				}
			}
		}
	}

	void recomputeDisulfides( chemfiles::Frame &				 p_frame,
							  const CellList &					 p_cellList,
							  const std::unordered_set<size_t> & p_sulfurAtoms )
	{
		const double maxDistanceForDisulfidesBond = 3.0;

		for ( const size_t sulfurAtom1 : p_sulfurAtoms )
		{
			const std::vector<size_t> & neighbours = p_cellList.getNeighbours( p_frame.positions()[ sulfurAtom1 ] );

			for ( const size_t neighbour : neighbours )
			{
				const std::vector<size_t> & sulfurVectorInNeighbour = p_cellList.getCysteineSulfurAtoms( neighbour );

				for ( const size_t sulfurAtom2 : sulfurVectorInNeighbour )
				{
					if ( sulfurAtom1 <= sulfurAtom2 )
						continue;

					// TODO : Change with sqrDistance when it will be added in chemfiles library
					const double dist = p_frame.distance( sulfurAtom1, sulfurAtom2 );

					if ( dist < maxDistanceForDisulfidesBond )
						p_frame.add_bond( sulfurAtom1, sulfurAtom2 );
				}
			}
		}
	}

	void recomputeBonds( chemfiles::Frame & p_frame, const VTX::Math::AABB & p_aabb )
	{
		const float cubeSize = 8.f; // = Biggest VdW radius * 2 round to sup
		CellList	cellList = CellList( p_aabb, cubeSize );

		std::unordered_set<size_t> sulfurAtoms = std::unordered_set<size_t>();

		for ( const chemfiles::Residue & residue : p_frame.topology().residues() )
		{
			const bool isStandard = Model::Residue::IsStandard( residue.name() );

			if ( isStandard )
			{
				const bool isCysteine = residue.name() == "CYS";
				for ( const size_t atomId : residue )
				{
					const chemfiles::Vector3D & atomPos			 = p_frame.positions()[ atomId ];
					const bool					isCysteineSulfur = isCysteine && p_frame[ atomId ].name() == "SG";

					if ( isCysteineSulfur )
					{
						cellList.addCysteineSulfur( atomId, atomPos );
						sulfurAtoms.emplace( atomId );
					}
					else
					{
						cellList.addAtom( atomId, atomPos );
					}
				}
			}
			else
			{
				for ( const size_t atomId : residue )
				{
					const chemfiles::Vector3D & atomPos = p_frame.positions()[ atomId ];
					cellList.addAtomFromNonStandardResidue( atomId, atomPos );
				}
			}
		}

		recomputeDisulfides( p_frame, cellList, sulfurAtoms );
		recomputeBondsOfNonStandardResidues( p_frame, cellList );
	}

	void recomputeBondsOfNonStandardResidues( chemfiles::Frame & frame, const CellList & p_cellList )
	{
		// float cutoff = 8.f;
		// float	  cutoffPow2		  = cutoff * cutoff;
		float	  cutoffPow2		  = 8.f;
		const int hydrogenSymbolValue = int( Model::Atom::SYMBOL::A_H );

		const std::vector<std::vector<size_t>> & atomsToCheck = p_cellList.getNonStdAtoms();

		for ( size_t cellIndex = 0; cellIndex < atomsToCheck.size(); cellIndex++ )
		{
			const size_t atomsInCell = atomsToCheck[ cellIndex ].size();

			for ( size_t nghb = 0; nghb < p_cellList.getNeighbourList()[ cellIndex ].size(); nghb++ )
			{
				size_t neighborCellIndex = p_cellList.getNeighbourList()[ cellIndex ][ nghb ];
				size_t atomNumInCell	 = p_cellList.getCellList()[ neighborCellIndex ].size();

				bool selfCell;
				if ( cellIndex == neighborCellIndex )
					selfCell = true;
				else
					selfCell = false;

				for ( size_t i = 0; i < atomsInCell; i++ )
				{
					const size_t			indexAtom1	= atomsToCheck[ cellIndex ][ i ];
					const chemfiles::Atom & atom1		= frame.topology()[ indexAtom1 ];
					const int				symbolAtom1 = atom1.atomic_number().value_or( 0 );

					for ( size_t j = 0; j < atomNumInCell; j++ )
					{
						const size_t indexAtom2 = p_cellList.getCellList()[ neighborCellIndex ][ j ];

						// TODO : Change with frame.sqrDistance when it will be added in chemfiles
						float interAtomicDist = frame.distance( indexAtom1, indexAtom2 );
						interAtomicDist *= interAtomicDist;

						// Perform distance test and ignore atoms with almost the same coordinates
						if ( ( interAtomicDist > cutoffPow2 ) || ( interAtomicDist < 0.03 ) )
						{
							continue;
						}

						const chemfiles::Atom & atom2		= frame.topology()[ indexAtom2 ];
						const int				symbolAtom2 = atom2.atomic_number().value_or( 0 );

						const float atom1Radius = Model::Atom::SYMBOL_VDW_RADIUS[ symbolAtom1 ];
						const float atom2Radius = Model::Atom::SYMBOL_VDW_RADIUS[ symbolAtom2 ];
						const float radii		= atom1Radius + atom2Radius;

						if ( interAtomicDist < radii )
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

	void recomputeBondOrders( chemfiles::Frame & p_frame )
	{
		std::vector<std::vector<size_t>> linkedAtomsVector = std::vector<std::vector<size_t>>();
		std::vector<CycleState>			 cycleStatePerAtom = std::vector<CycleState>();
		buildNeighbourStruct( p_frame, linkedAtomsVector );

		tagCycles( p_frame, linkedAtomsVector, cycleStatePerAtom );

		checkBondOrders( p_frame, linkedAtomsVector, cycleStatePerAtom );
	}

	void buildNeighbourStruct( const chemfiles::Frame &			  p_frame,
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

	void tagCycles( chemfiles::Frame &						 p_frame,
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
	void _tagCyclesRecursive( const chemfiles::Frame &				   p_frame,
							  const std::vector<std::vector<size_t>> & p_linkedAtomsVector,
							  std::vector<CycleState> &				   p_cycleStatePerAtoms,
							  std::vector<size_t> &					   p_cycleIndexes,
							  short									   p_counter )
	{
		const size_t & currentAtomIndex = p_cycleIndexes[ p_counter - 1 ];
		for ( const size_t nextAtomIndex : p_linkedAtomsVector[ currentAtomIndex ] )
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
					const float avgDotCross = computeAverageRingDotCross( p_frame, p_cycleIndexes, p_counter, dir );
					const bool	isPlanar
						= avgDotCross > 0.96f
						  && verifyPlanarBonds( p_frame, p_cycleIndexes, p_counter, p_linkedAtomsVector, dir, 0.35f );

					CycleState cycleState = isPlanar ? CycleState::Planar : CycleState::Cyclic;

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

	void checkBondOrders( chemfiles::Frame &					   p_frame,
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

				for ( size_t nextAtomIndex : linkedAtoms )
				{
					const chemfiles::Vector3D & nextAtomIndexPos = p_frame.positions()[ nextAtomIndex ];
					const float					atomDistance	 = ( nextAtomIndexPos - atomIndexPos ).norm();

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
					const float avgDotCross = computeAverageCenterDotCross( p_frame, linkedAtoms );

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
								const size_t firstOxygenIndex = neighbourData.oxygens[ 0 ].index;

								if ( neighbourData.oxygens[ 0 ].distance < 1.38f
									 && p_linkedAtomsVector[ firstOxygenIndex ].size() == 1 )
								{
									setBondOrder(
										p_frame, atomIndex, firstOxygenIndex, chemfiles::Bond::BondOrder::DOUBLE );
								}
							}
							else if ( nitrogenCountInNeighbours >= 1 && oxygenCountInNeighbours >= 2 )
							{
								// Carbamyl
								const size_t firstOxygenIndex  = neighbourData.oxygens[ 0 ].index;
								const size_t secondOxygenIndex = neighbourData.oxygens[ 1 ].index;

								const bool firstOxygenIsValid = neighbourData.oxygens[ 0 ].distance < 1.38f
																&& p_linkedAtomsVector[ firstOxygenIndex ].size() == 1;
								const bool secondOxygenIsValid = neighbourData.oxygens[ 0 ].distance < 1.38f
																 && p_linkedAtomsVector[ firstOxygenIndex ].size() == 1;

								if ( firstOxygenIsValid && secondOxygenIsValid )
								{
									// order = 4 in pymol
									setBondOrder(
										p_frame, atomIndex, firstOxygenIndex, chemfiles::Bond::BondOrder::DOUBLE );
									// order = 4 in pymol
									setBondOrder(
										p_frame, atomIndex, secondOxygenIndex, chemfiles::Bond::BondOrder::SINGLE );
								}
								else if ( firstOxygenIsValid )
								{
									setBondOrder(
										p_frame, atomIndex, firstOxygenIndex, chemfiles::Bond::BondOrder::DOUBLE );
								}
								else if ( secondOxygenIsValid )
								{
									setBondOrder(
										p_frame, atomIndex, secondOxygenIndex, chemfiles::Bond::BondOrder::DOUBLE );
								}
							}
							else if ( nitrogenCountInNeighbours == 0 && oxygenCountInNeighbours == 1 )
							{
								// Ketone
								const size_t firstOxygenIndex = neighbourData.oxygens[ 0 ].index;

								if ( neighbourData.oxygens[ 0 ].distance < 1.31f
									 && p_linkedAtomsVector[ firstOxygenIndex ].size() == 1 )
								{
									setBondOrder(
										p_frame, atomIndex, firstOxygenIndex, chemfiles::Bond::BondOrder::DOUBLE );
								}
							}
							else if ( nitrogenCountInNeighbours == 0 && oxygenCountInNeighbours >= 2 )
							{
								// Simple Carboxylate ?
								const size_t firstOxygenIndex  = neighbourData.oxygens[ 0 ].index;
								const size_t secondOxygenIndex = neighbourData.oxygens[ 1 ].index;

								const bool firstOxygenIsValid = neighbourData.oxygens[ 0 ].distance < 1.38f
																&& p_linkedAtomsVector[ firstOxygenIndex ].size() == 1;
								const bool secondOxygenIsValid = neighbourData.oxygens[ 0 ].distance < 1.38f
																 && p_linkedAtomsVector[ firstOxygenIndex ].size() == 1;

								if ( firstOxygenIsValid && secondOxygenIsValid )
								{
									// ASP | GLU
									// order = 4 in pymol
									setBondOrder(
										p_frame, atomIndex, firstOxygenIndex, chemfiles::Bond::BondOrder::DOUBLE );
									// order = 4 in pymol
									setBondOrder(
										p_frame, atomIndex, secondOxygenIndex, chemfiles::Bond::BondOrder::SINGLE );
								}
								else if ( firstOxygenIsValid )
								{
									// Esther
									setBondOrder(
										p_frame, atomIndex, firstOxygenIndex, chemfiles::Bond::BondOrder::DOUBLE );
								}
								else if ( secondOxygenIsValid )
								{
									setBondOrder(
										p_frame, atomIndex, secondOxygenIndex, chemfiles::Bond::BondOrder::DOUBLE );
								}
							}
							else if ( nitrogenCountInNeighbours >= 3 )
							{
								// Guanido with no hydrogen
								if ( neighbourData.nitrogens[ 0 ].distance < 1.44f
									 && neighbourData.nitrogens[ 1 ].distance < 1.44f
									 && neighbourData.nitrogens[ 2 ].distance < 1.44f )
								{
									const size_t n0Index = neighbourData.nitrogens[ 0 ].index;
									const size_t n1Index = neighbourData.nitrogens[ 1 ].index;
									const size_t n2Index = neighbourData.nitrogens[ 2 ].index;

									const size_t n0NeighbourCount = p_linkedAtomsVector[ n0Index ].size();
									const size_t n1NeighbourCount = p_linkedAtomsVector[ n1Index ].size();
									const size_t n2NeighbourCount = p_linkedAtomsVector[ n2Index ].size();

									if ( n0NeighbourCount == 1 && n1NeighbourCount == 1 && n2NeighbourCount >= 2 )
									{
										// order = 4 in pymol
										setBondOrder( p_frame, atomIndex, n0Index, chemfiles::Bond::BondOrder::SINGLE );
										// order = 4 in pymol
										setBondOrder( p_frame, atomIndex, n1Index, chemfiles::Bond::BondOrder::SINGLE );
									}
									else if ( n0NeighbourCount == 1 && n1NeighbourCount >= 2 && n2NeighbourCount == 1 )
									{
										// order = 4 in pymol
										setBondOrder( p_frame, atomIndex, n0Index, chemfiles::Bond::BondOrder::SINGLE );
										// order = 4 in pymol
										setBondOrder( p_frame, atomIndex, n2Index, chemfiles::Bond::BondOrder::SINGLE );
									}
									else if ( n0NeighbourCount >= 2 && n1NeighbourCount == 1 && n2NeighbourCount == 1 )
									{
										// ARG
										// order = 4 in pymol
										setBondOrder( p_frame, atomIndex, n1Index, chemfiles::Bond::BondOrder::SINGLE );
										// order = 4 in pymol
										setBondOrder( p_frame, atomIndex, n2Index, chemfiles::Bond::BondOrder::DOUBLE );
									}
								}
							}
							else if ( nitrogenCountInNeighbours == 2 && neighbourData.carbons.size() > 0 )
							{
								if ( neighbourData.nitrogens[ 0 ].distance < 1.43f
									 && neighbourData.nitrogens[ 1 ].distance < 1.43f )
								{
									const size_t n0Index = neighbourData.nitrogens[ 0 ].index;
									const size_t n1Index = neighbourData.nitrogens[ 1 ].index;

									if ( p_cycleStatePerAtom[ neighbourData.carbons[ 0 ].index ] == CycleState::Planar
										 && p_cycleStatePerAtom[ atomIndex ] == CycleState::None
										 && p_cycleStatePerAtom[ n0Index ] == CycleState::None
										 && p_cycleStatePerAtom[ n1Index ] == CycleState::None )
									{
										// order = 4 in pymol
										setBondOrder( p_frame, atomIndex, n0Index, chemfiles::Bond::BondOrder::SINGLE );
										// order = 4 in pymol
										setBondOrder( p_frame, atomIndex, n1Index, chemfiles::Bond::BondOrder::SINGLE );
									}
								}
							}
						}
					}

					// Any carbons
					// Imines and Nitriles
					if ( ( neighbourCount == 2 || ( neighbourCount == 3 && avgDotCross > planerCutoff ) )
						 && neighbourData.nitrogens.size() == 1 && neighbourData.oxygens.size() == 0
						 && neighbourData.nitrogens[ 0 ].distance < 1.36f
						 && p_cycleStatePerAtom[ neighbourData.nitrogens[ 0 ].index ] == CycleState::None
						 && p_cycleStatePerAtom[ atomIndex ] == CycleState::None )
					{
						const size_t n0Index	= neighbourData.nitrogens[ 0 ].index;
						float		 n0DotCross = 1.0f;
						if ( p_linkedAtomsVector[ n0Index ].size() == 3 )
						{
							std::vector<size_t> atomsForPlanarity = std::vector<size_t>();
							atomsForPlanarity.resize( 4 );
							atomsForPlanarity[ 0 ] = n0Index;

							for ( int i = 0; i <= 3; i++ )
							{
								atomsForPlanarity[ i + 1 ] = p_linkedAtomsVector[ n0Index ][ i ];
							}

							n0DotCross = computeAverageCenterDotCross( p_frame, atomsForPlanarity );
						}

						if ( n0DotCross > planerCutoff )
						{
							setBondOrder( p_frame, atomIndex, n0Index, chemfiles::Bond::BondOrder::DOUBLE );
							if ( neighbourData.nitrogens[ 0 ].distance < 1.24f && neighbourData.carbons.size() > 0
								 && p_linkedAtomsVector[ n0Index ].size() == 1 )
							{
								chemfiles::Vector3D n0Vector = neighbourData.nitrogens[ 0 ].position - atomIndexPos;
								normalizeVector( n0Vector );
								chemfiles::Vector3D c0Vector = neighbourData.carbons[ 0 ].position - atomIndexPos;
								normalizeVector( c0Vector );

								if ( chemfiles::dot( n0Vector, c0Vector ) < -0.9 )
								{
									setBondOrder( p_frame, atomIndex, n0Index, chemfiles::Bond::BondOrder::TRIPLE );
								}
							}
						}
					}
				}
				else if ( atomType == "N" )
				{
					if ( neighbourCount == 3 && neighbourData.oxygens.size() == 2
						 && computeAverageCenterDotCross( p_frame, linkedAtoms ) > planerCutoff )
					{
						const size_t firstOxygenIndex = neighbourData.oxygens[ 0 ].index;
						if ( p_linkedAtomsVector[ firstOxygenIndex ].size() == 1 )
						{
							// order = 4 in pymol
							setBondOrder( p_frame, atomIndex, firstOxygenIndex, chemfiles::Bond::BondOrder::SINGLE );
						}

						const size_t secondOxygenIndex = neighbourData.oxygens[ 1 ].index;
						if ( p_linkedAtomsVector[ secondOxygenIndex ].size() == 1 )
						{
							// order = 4 in pymol
							setBondOrder( p_frame, atomIndex, secondOxygenIndex, chemfiles::Bond::BondOrder::SINGLE );
						}
					}
				}
				else if ( atomType == "S" || atomType == "P" )
				{
					if ( neighbourData.oxygens.size() >= 4 )
					{
						for ( size_t i = 0; i < neighbourData.oxygens.size(); i++ )
						{
							const size_t oxygenIndex = neighbourData.oxygens[ i ].index;
							if ( p_linkedAtomsVector[ oxygenIndex ].size() == 1 )
							{
								setBondOrder( p_frame, atomIndex, oxygenIndex, chemfiles::Bond::BondOrder::DOUBLE );
								break;
							}
						}
					}
					else if ( neighbourData.oxygens.size() >= 2 )
					{
						for ( size_t i = 0; i < neighbourData.oxygens.size(); i++ )
						{
							const size_t oxygenIndex = neighbourData.oxygens[ i ].index;
							if ( p_linkedAtomsVector[ oxygenIndex ].size() == 1 )
							{
								setBondOrder( p_frame, atomIndex, oxygenIndex, chemfiles::Bond::BondOrder::DOUBLE );
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
									setBondOrder(
										p_frame, atomIndex, neighbourIndex, chemfiles::Bond::BondOrder::AROMATIC );
								}
							}
						}
					}
				}
			}
		}
	}

	void normalizeVector( chemfiles::Vector3D & p_vector )
	{
		const double dist = p_vector.norm();
		p_vector[ 0 ] /= dist;
		p_vector[ 1 ] /= dist;
		p_vector[ 2 ] /= dist;
	}

	float computeAverageCenterDotCross( const chemfiles::Frame & p_frame, const std::vector<size_t> & p_atoms )
	{
		float		 result	   = 0.0F;
		const size_t atomCount = p_atoms.size();

		if ( atomCount == 0 )
			return result;

		std::vector<chemfiles::Vector3D> positions = std::vector<chemfiles::Vector3D>();
		positions.resize( atomCount + 1 );

		for ( int i = 0; i < atomCount; i++ )
			positions[ i ] = p_frame.positions()[ p_atoms[ i ] ];

		std::vector<chemfiles::Vector3D> crossProducts = std::vector<chemfiles::Vector3D>();
		crossProducts.resize( atomCount + 1 );

		positions[ atomCount ] = positions[ 1 ];

		for ( int i = 1; i < atomCount; i++ )
		{
			chemfiles::Vector3D vec0 = positions[ i ] - positions[ 0 ];
			normalizeVector( vec0 );
			chemfiles::Vector3D vec1 = positions[ i + 1 ] - positions[ 0 ];
			normalizeVector( vec1 );

			crossProducts[ i ] = chemfiles::cross( vec0, vec1 );
			normalizeVector( crossProducts[ i ] );

			if ( i > 1 && chemfiles::dot( crossProducts[ i - 1 ], crossProducts[ i ] ) < 0.0 )
				crossProducts[ i ] = -crossProducts[ i ];
		}
		crossProducts[ atomCount ] = crossProducts[ 1 ];

		float avg = 0.0;
		for ( int i = 1; i < atomCount; i++ )
		{
			avg += chemfiles::dot( crossProducts[ i ], crossProducts[ i + 1 ] );
		}

		result = avg / ( atomCount - 1 );
		return result;
	}
	float computeAverageRingDotCross( const chemfiles::Frame &	  p_frame,
									  const std::vector<size_t> & p_atoms,
									  const int					  p_atomCount,
									  chemfiles::Vector3D &		  dir )
	{
		float							 result	   = 0.0F;
		const size_t					 atomCount = p_atomCount;
		std::vector<chemfiles::Vector3D> positions = std::vector<chemfiles::Vector3D>();
		positions.resize( p_atomCount + 2 );

		for ( int i = 0; i < p_atomCount; i++ )
			positions[ i ] = p_frame.positions()[ p_atoms[ i ] ];

		std::vector<chemfiles::Vector3D> crossProducts = std::vector<chemfiles::Vector3D>();
		crossProducts.resize( p_atomCount + 1 );

		float avg = 0.0;

		positions[ p_atomCount ]	 = positions[ 0 ];
		positions[ p_atomCount + 1 ] = positions[ 1 ];

		for ( int i = 0; i < p_atomCount; i++ )
		{
			chemfiles::Vector3D vec0 = positions[ i ] - positions[ i + 1 ];
			normalizeVector( vec0 );
			chemfiles::Vector3D vec1 = positions[ i + 2 ] - positions[ i + 1 ];
			normalizeVector( vec0 );

			crossProducts[ i ] = chemfiles::cross( vec0, vec1 );
			normalizeVector( crossProducts[ i ] );

			if ( i > 1 && chemfiles::dot( crossProducts[ i - 1 ], crossProducts[ i ] ) < 0.0 )
				crossProducts[ i ] = -crossProducts[ i ];

			dir += crossProducts[ i ];
		}

		crossProducts[ p_atomCount ] = crossProducts[ 0 ];

		for ( int i = 0; i < p_atomCount; i++ )
		{
			avg += chemfiles::dot( crossProducts[ i ], crossProducts[ i + 1 ] );
		}
		result = avg / p_atomCount;
		normalizeVector( dir );

		return result;
	}

	bool verifyPlanarBonds( const chemfiles::Frame &				 p_frame,
							const std::vector<size_t> &				 p_atoms,
							const int								 p_atomCount,
							const std::vector<std::vector<size_t>> & p_linkedAtomsVector,
							const chemfiles::Vector3D &				 dir,
							const float								 cutoff )
	{
		for ( int i = 0; i < p_atomCount; i++ )
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
				normalizeVector( vec );
				float dot = chemfiles::dot( vec, dir );
				dot		  = dot < 0 ? -dot : dot;

				if ( dot > cutoff )
					return false;
			}
		}

		return true;
	}

	void setBondOrder( chemfiles::Frame &				p_frame,
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
} // namespace VTX::Util::Chemfiles
