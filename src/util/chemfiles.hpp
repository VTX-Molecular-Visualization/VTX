#ifndef __VTX_UTIL_CHEMFILES__
#define __VTX_UTIL_CHEMFILES__

#include "math/aabb.hpp"
#include "util/filesystem.hpp"
#pragma warning( push, 0 )
#include <chemfiles.hpp>
#pragma warning( pop )
#include <algorithm>
#include <string>
#include <unordered_set>

namespace VTX::Util::Chemfiles
{
	enum CycleState : int
	{
		None   = 0,
		Cyclic = 1,
		Planar = 2,
	};

	struct CellList
	{
	  public:
		CellList( const VTX::Math::AABB & p_aabb, const float p_cubeSize ) :
			_cubeSize( p_cubeSize ), _xStart( p_aabb.getMin().x ), _yStart( p_aabb.getMin().y ),
			_zStart( p_aabb.getMin().z ), _width( int( ( p_aabb.getMax().x - p_aabb.getMin().x ) / p_cubeSize ) + 1 ),
			_height( int( ( p_aabb.getMax().y - p_aabb.getMin().y ) / p_cubeSize ) + 1 ),
			_depth( int( ( p_aabb.getMax().z - p_aabb.getMin().z ) / p_cubeSize ) + 1 )
		{
			const size_t listSize = _width * _height * _depth;
			_cellList.resize( listSize );
			_generateNeighbourList();
			_cysteinSulfurIndexes.resize( listSize );
			_nonStdAtoms.resize( listSize );
		}

		void clear() { std::fill( _cellList.begin(), _cellList.end(), std::vector<size_t>() ); }

		void addAtom( const size_t p_atomIndex, const chemfiles::Vector3D & p_position );
		void addAtomFromNonStandardResidue( const size_t p_atomIndex, const chemfiles::Vector3D & p_position );
		void addCysteineSulfur( const size_t p_atomIndex, const chemfiles::Vector3D & p_position );

		const std::vector<size_t> &				 getCysteineSulfurAtoms( const size_t p_cellIndex ) const;
		const std::vector<std::vector<size_t>> & getCysteineSulfurAtoms() const;

		std::vector<std::vector<size_t>> &		 getNonStdAtoms();
		const std::vector<std::vector<size_t>> & getNonStdAtoms() const;

		const std::vector<size_t> & getNeighbours( const size_t & p_index ) const;
		const std::vector<size_t> & getNeighbours( const chemfiles::Vector3D & p_position ) const;

		const std::vector<std::vector<size_t>> & getCellList() const { return _cellList; }
		std::vector<std::vector<size_t>> &		 getCellList() { return _cellList; }
		const std::vector<std::vector<size_t>> & getNeighbourList() const { return _neighbourList; }
		std::vector<std::vector<size_t>> &		 getNeighbourList() { return _neighbourList; }

	  private:
		std::vector<std::vector<size_t>> _cellList			   = std::vector<std::vector<size_t>>();
		std::vector<std::vector<size_t>> _neighbourList		   = std::vector<std::vector<size_t>>();
		std::vector<std::vector<size_t>> _cysteinSulfurIndexes = std::vector<std::vector<size_t>>();
		std::vector<std::vector<size_t>> _nonStdAtoms		   = std::vector<std::vector<size_t>>();

		const float _cubeSize;

		const size_t _width;
		const size_t _height;
		const size_t _depth;

		const float _xStart;
		const float _yStart;
		const float _zStart;

		size_t _getIndexFromPosition( const chemfiles::Vector3D & p_position ) const;
		void   _generateNeighbourList();
	};
	struct NeighbourData
	{
	  public:
		struct AtomData
		{
			AtomData() {}
			AtomData( const size_t p_index, const chemfiles::Vector3D & p_position, const float p_distance ) :
				index( p_index ), position( p_position ), distance( p_distance )
			{
			}

			size_t				index;
			chemfiles::Vector3D position;
			float				distance;
		};

		NeighbourData()
		{
			oxygens.reserve( 4 );
			carbons.reserve( 2 );
			nitrogens.reserve( 3 );
		}

		std::vector<AtomData> oxygens	= std::vector<AtomData>();
		std::vector<AtomData> carbons	= std::vector<AtomData>();
		std::vector<AtomData> nitrogens = std::vector<AtomData>();
	};

	void recomputeBonds( chemfiles::Frame & p_frame, const VTX::Math::AABB & p_aabb );

	void recomputeBondsOfNonStandardResidues( chemfiles::Frame & frame, const CellList & p_cellList );

	void recomputeDisulfides( chemfiles::Frame &				 p_frame,
							  const CellList &					 p_cellList,
							  const std::unordered_set<size_t> & p_sulphurAtoms );

	void recomputeBondOrders( chemfiles::Frame & p_frame );

	void buildNeighbourStruct( const chemfiles::Frame &			  p_frame,
							   std::vector<std::vector<size_t>> & p_linkedAtomsVector );

	void checkBondOrders( chemfiles::Frame &					   p_frame,
						  const std::vector<std::vector<size_t>> & p_linkedAtomsVector,
						  const std::vector<CycleState> &		   p_cycleStatePerAtom );

	void tagCycles( chemfiles::Frame &						 p_frame,
					const std::vector<std::vector<size_t>> & p_linkedAtomsVector,
					std::vector<CycleState> &				 p_cycleStatePerAtom );

	void _tagCyclesRecursive( const chemfiles::Frame &				   p_frame,
							  const std::vector<std::vector<size_t>> & p_linkedAtomsVector,
							  std::vector<CycleState> &				   p_cycleStatePerAtom,
							  std::vector<size_t> &					   p_cycleIndexes,
							  short									   p_counter );

	float computeAverageCenterDotCross( const chemfiles::Frame & p_frame, const std::vector<size_t> & p_atoms );
	float computeAverageRingDotCross( const chemfiles::Frame &	  p_frame,
									  const std::vector<size_t> & p_atoms,
									  const int					  p_atomCount,
									  chemfiles::Vector3D &		  dir );
	bool  verifyPlanarBonds( const chemfiles::Frame &				  p_frame,
							 const std::vector<size_t> &			  p_atoms,
							 const int								  p_atomCount,
							 const std::vector<std::vector<size_t>> & p_linkedAtomsVector,
							 const chemfiles::Vector3D &			  dir,
							 const float							  cutoff );
	void  normalizeVector( chemfiles::Vector3D & p_vector );

	void setBondOrder( chemfiles::Frame &				p_frame,
					   const size_t						p_firstAtomIndex,
					   const size_t						p_secondAtomIndex,
					   const chemfiles::Bond::BondOrder p_bondOrder,
					   const bool						p_force = false );

} // namespace VTX::Util::Chemfiles

#endif
