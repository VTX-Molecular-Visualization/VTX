#ifndef __VTX_UTIL_BOND_GUESSING_RECOMPUTATION__
#define __VTX_UTIL_BOND_GUESSING_RECOMPUTATION__

#include "model/molecule.hpp"
#include "object3d/helper/aabb.hpp"
#include "util/filesystem.hpp"
#pragma warning( push, 0 )
#include <chemfiles.hpp>
#pragma warning( pop )
#include <algorithm>
#include <string>
#include <unordered_set>

namespace VTX::Util::BondGuessing
{
	class BondRecomputation
	{
	  private:
		struct CellList
		{
		  public:
			CellList( const VTX::Object3D::Helper::AABB & p_aabb, const float p_cubeSize ) :
				_cubeSize( p_cubeSize ), _xStart( p_aabb.getMin().x ), _yStart( p_aabb.getMin().y ),
				_zStart( p_aabb.getMin().z ),
				_width( int( ( p_aabb.getMax().x - p_aabb.getMin().x ) / p_cubeSize ) + 1 ),
				_height( int( ( p_aabb.getMax().y - p_aabb.getMin().y ) / p_cubeSize ) + 1 ),
				_depth( int( ( p_aabb.getMax().z - p_aabb.getMin().z ) / p_cubeSize ) + 1 )
			{
				const size_t listSize = size_t( _width * _height * _depth );
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

			const uint _width;
			const uint _height;
			const uint _depth;

			const float _xStart;
			const float _yStart;
			const float _zStart;

			size_t _getIndexFromPosition( const chemfiles::Vector3D & p_position ) const;
			void   _generateNeighbourList();
		};

	  public:
		static void recomputeBonds( chemfiles::Frame & p_frame, const VTX::Object3D::Helper::AABB & p_aabb );

	  private:
		inline static const double MAX_DISTANCE_FOR_DISULFIDE_BOND = 3.0;
		inline static const double MAX_SQR_DISTANCE_FOR_DISULFIDE_BOND
			= MAX_DISTANCE_FOR_DISULFIDE_BOND * MAX_DISTANCE_FOR_DISULFIDE_BOND;

		static void _recomputeBondsOfNonStandardResidues( chemfiles::Frame & frame, const CellList & p_cellList );
		static void _recomputeDisulfides( chemfiles::Frame &				 p_frame,
										  const CellList &					 p_cellList,
										  const std::unordered_set<size_t> & p_sulphurAtoms );

		static double _sqrDistance( const chemfiles::Vector3D & p_lhs, const chemfiles::Vector3D & p_rhs );
	};

	bool shouldRecomputeBonds( const chemfiles::Frame & p_frame );
} // namespace VTX::Util::BondGuessing

#endif
