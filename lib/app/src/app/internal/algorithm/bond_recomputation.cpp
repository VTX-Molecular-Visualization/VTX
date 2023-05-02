#include "app/internal/algorithm/bond_recomputation.hpp"
#include "app/application/setting.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/internal/chemdb/atom.hpp"
#include <iostream>

namespace VTX::App::Internal::Algorithm
{
	void BondRecomputation::CellList::addAtom( const size_t p_atomIndex, const chemfiles::Vector3D & p_position )
	{
		const size_t atomIndex = _getIndexFromPosition( p_position );
		_cellList[ atomIndex ].emplace_back( p_atomIndex );
	}
	void BondRecomputation::CellList::addAtomFromNonStandardResidue( const size_t				 p_atomIndex,
																	 const chemfiles::Vector3D & p_position )
	{
		const size_t cellIndex = _getIndexFromPosition( p_position );
		_cellList[ cellIndex ].emplace_back( p_atomIndex );
		_nonStdAtoms[ cellIndex ].emplace_back( p_atomIndex );
	}

	void BondRecomputation::CellList::addCysteineSulfur( const size_t				 p_atomIndex,
														 const chemfiles::Vector3D & p_position )
	{
		const size_t cellIndex = _getIndexFromPosition( p_position );
		_cellList[ cellIndex ].emplace_back( p_atomIndex );
		_cysteinSulfurIndexes[ cellIndex ].emplace_back( p_atomIndex );
	}

	const std::vector<size_t> & BondRecomputation::CellList::getNeighbours( const size_t & p_index ) const
	{
		return _neighbourList[ p_index ];
	}
	const std::vector<size_t> & BondRecomputation::CellList::getNeighbours(
		const chemfiles::Vector3D & p_position ) const
	{
		const size_t cellIndex = _getIndexFromPosition( p_position );
		return _neighbourList[ cellIndex ];
	}
	const std::vector<size_t> & BondRecomputation::CellList::getCysteineSulfurAtoms( const size_t p_cellIndex ) const
	{
		return _cysteinSulfurIndexes[ p_cellIndex ];
	}
	const std::vector<std::vector<size_t>> & BondRecomputation::CellList::getCysteineSulfurAtoms() const
	{
		return _cysteinSulfurIndexes;
	}
	const std::vector<std::vector<size_t>> & BondRecomputation::CellList::getNonStdAtoms() const
	{
		return _nonStdAtoms;
	}
	std::vector<std::vector<size_t>> & BondRecomputation::CellList::getNonStdAtoms() { return _nonStdAtoms; }

	size_t BondRecomputation::CellList::_getIndexFromPosition( const chemfiles::Vector3D & p_position ) const
	{
		const size_t xIndex = int( ( p_position[ 0 ] - _xStart ) / _cubeSize );
		const size_t yIndex = int( ( p_position[ 1 ] - _yStart ) / _cubeSize );
		const size_t zIndex = int( ( p_position[ 2 ] - _zStart ) / _cubeSize );

		return xIndex * _height * _depth + yIndex * _depth + zIndex;
	}
	void BondRecomputation::CellList::_generateNeighbourList()
	{
		_neighbourList = std::vector<std::vector<size_t>>();
		_neighbourList.resize( _cellList.size() );

		const size_t xOffset = _height * _depth;
		const size_t yOffset = _depth;
		const size_t zOffset = 1;

		for ( uint i = 0; i < _width; i++ )
		{
			for ( uint j = 0; j < _height; j++ )
			{
				for ( uint k = 0; k < _depth; k++ )
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

	void BondRecomputation::recomputeBonds( chemfiles::Frame &							   p_frame,
											const App::Component::Object3D::Helper::AABB & p_aabb )
	{
		CellList cellList = CellList( p_aabb, VTX::App::Application::Setting::CELL_LIST_CUBE_SIZE );

		std::unordered_set<size_t> sulfurAtoms = std::unordered_set<size_t>();

		for ( const chemfiles::Residue & residue : p_frame.topology().residues() )
		{
			const bool isStandard = App::Component::Chemistry::Residue::checkIfStandardFromName( residue.name() );

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

		_recomputeDisulfides( p_frame, cellList, sulfurAtoms );
		_recomputeBondsOfNonStandardResidues( p_frame, cellList );
	}

	void BondRecomputation::_recomputeDisulfides( chemfiles::Frame &				 p_frame,
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

	void BondRecomputation::_recomputeBondsOfNonStandardResidues( chemfiles::Frame & frame,
																  const CellList &	 p_cellList )
	{
		// float cutoff = 8.f;
		// float	  cutoffPow2		  = cutoff * cutoff;
		float	  cutoffPow2		  = 8.f;
		const int hydrogenSymbolValue = int( App::Internal::ChemDB::Atom::SYMBOL::A_H );

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
					const int				symbolAtom1 = int( atom1.atomic_number().value_or( 0 ) );

					for ( size_t j = 0; j < atomNumInCell; j++ )
					{
						const size_t indexAtom2 = p_cellList.getCellList()[ neighborCellIndex ][ j ];

						// TODO : Change with frame.sqrDistance when it will be added in chemfiles
						const float interAtomicDist
							= float( ( frame.positions()[ indexAtom2 ] - frame.positions()[ indexAtom1 ] ).sqrNorm() );
						// float interAtomicDist = frame.distance( indexAtom1, indexAtom2 );
						// interAtomicDist *= interAtomicDist;

						// Perform distance test and ignore atoms with almost the same coordinates
						if ( ( interAtomicDist > cutoffPow2 ) || ( interAtomicDist < 0.03 ) )
						{
							continue;
						}

						const chemfiles::Atom & atom2		= frame.topology()[ indexAtom2 ];
						const int				symbolAtom2 = int( atom2.atomic_number().value_or( 0 ) );

						const float atom1Radius = App::Internal::ChemDB::Atom::SYMBOL_VDW_RADIUS[ symbolAtom1 ];
						const float atom2Radius = App::Internal::ChemDB::Atom::SYMBOL_VDW_RADIUS[ symbolAtom2 ];
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

} // namespace VTX::App::Internal::Algorithm
