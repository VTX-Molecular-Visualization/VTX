#include "bond_recomputation.hpp"
#include "model/atom.hpp"
#include "model/residue.hpp"
#include "setting.hpp"
#include <iostream>

namespace VTX::Util::BondGuessing
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

	void BondRecomputation::recomputeBonds( chemfiles::Frame & p_frame, const VTX::Object3D::Helper::AABB & p_aabb )
	{
		CellList cellList = CellList( p_aabb, Setting::CELL_LIST_CUBE_SIZE );

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

					cellList.addAtom( atomId, atomPos );
				}
			}
			else
			{
				for ( const size_t atomId : residue )
				{
					const chemfiles::Vector3D & atomPos	 = p_frame.positions()[ atomId ];
					const bool					isSulfur = p_frame[ atomId ].type() == "S";

					if ( isSulfur )
					{
						cellList.addCysteineSulfur( atomId, atomPos );
						sulfurAtoms.emplace( atomId );
					}

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

					const double sqrDist
						= _sqrDistance( p_frame.positions()[ sulfurAtom1 ], p_frame.positions()[ sulfurAtom2 ] );

					if ( sqrDist < MAX_SQR_DISTANCE_FOR_DISULFIDE_BOND )
						p_frame.add_bond( sulfurAtom1, sulfurAtom2 );
				}
			}
		}
	}

	// The idea is to hard code a list of exceptions for atoms that are most of the time dissolved ions iin biological
	// context
	bool isUnbondingAtoms( const int & p_atomNumber )
	{
		using namespace VTX::Model;
		const Atom::SYMBOL atomSymbol = static_cast<Atom::SYMBOL>( p_atomNumber );
		switch ( atomSymbol )
		{
		case Atom::SYMBOL::A_NA: return true;
		case Atom::SYMBOL::A_K: return true;
		// case Atom::SYMBOL::A_CL: return false;
		case Atom::SYMBOL::A_MG: return true;
		case Atom::SYMBOL::A_ZN: return true;
		case Atom::SYMBOL::A_MN: return true;
		default: return false;
		}
	}

	void BondRecomputation::_recomputeBondsOfNonStandardResidues( chemfiles::Frame & frame,
																  const CellList &	 p_cellList )
	{
		const float cutoff				= 3.48f * 2.f;
		const float cutoffPow2			= cutoff * cutoff;
		const int	hydrogenSymbolValue = int( Model::Atom::SYMBOL::A_H );

		const std::vector<std::vector<size_t>> & atomsToCheck = p_cellList.getNonStdAtoms();

		for ( size_t cellIndex = 0; cellIndex < atomsToCheck.size(); cellIndex++ )
		{
			const size_t atomsInCell = atomsToCheck[ cellIndex ].size();

			for ( size_t nghb = 0; nghb < p_cellList.getNeighbourList()[ cellIndex ].size(); nghb++ )
			{
				const size_t neighborCellIndex = p_cellList.getNeighbourList()[ cellIndex ][ nghb ];
				const size_t atomNumInCell	   = p_cellList.getCellList()[ neighborCellIndex ].size();

				const bool selfCell = ( cellIndex == neighborCellIndex );

				for ( size_t i = 0; i < atomsInCell; i++ )
				{
					const size_t			indexAtom1	= atomsToCheck[ cellIndex ][ i ];
					const chemfiles::Atom & atom1		= frame.topology()[ indexAtom1 ];
					const int				symbolAtom1 = atom1.atomic_number().value_or( 0 );

					if ( isUnbondingAtoms( symbolAtom1 ) )
						continue;

					for ( size_t j = 0; j < atomNumInCell; j++ )
					{
						const size_t indexAtom2 = p_cellList.getCellList()[ neighborCellIndex ][ j ];

						const float interAtomicSqrDist
							= _sqrDistance( frame.positions()[ indexAtom1 ], frame.positions()[ indexAtom2 ] );

						// Perform distance test and ignore atoms with almost the same coordinates
						if ( ( interAtomicSqrDist > cutoffPow2 ) || ( interAtomicSqrDist < 0.03 ) )
						{
							continue;
						}

						const chemfiles::Atom & atom2		= frame.topology()[ indexAtom2 ];
						const int				symbolAtom2 = atom2.atomic_number().value_or( 0 );

						if ( isUnbondingAtoms( symbolAtom2 ) )
							continue;

						const float atom1Radius		  = Model::Atom::SYMBOL_VDW_RADIUS[ symbolAtom1 ];
						const float atom2Radius		  = Model::Atom::SYMBOL_VDW_RADIUS[ symbolAtom2 ];
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

	double BondRecomputation::_sqrDistance( const chemfiles::Vector3D & p_lhs, const chemfiles::Vector3D & p_rhs )
	{
		return ( p_rhs - p_lhs ).sqrNorm();
	}

	bool shouldRecomputeBonds( const chemfiles::Frame & p_frame ) { return not p_frame.has_bond(); }

} // namespace VTX::Util::BondGuessing
