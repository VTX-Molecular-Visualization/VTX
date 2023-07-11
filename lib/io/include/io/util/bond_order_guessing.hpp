#ifndef __VTX_IO_UTIL_BOND_ORDER_GUESSING__
#define __VTX_IO_UTIL_BOND_ORDER_GUESSING__

#include "io/reader/residue_data_reader.hpp"
#include <core/chemdb/bond.hpp>
#pragma warning( push, 0 )
#include <chemfiles.hpp>
#pragma warning( pop )
#include <map>
#include <string>
#include <vector>

namespace VTX::IO::Util
{
	class BondOrderGuessing
	{
	  private:
		enum CycleState : int
		{
			None   = 0,
			Cyclic = 1,
			Planar = 2,
		};
		struct NeighbourData
		{
		  public:
			struct AtomData
			{
			  private:
				union position
				{
					position( const chemfiles::Vector3D & p_position ) : chemfiles( p_position ) {};
					position( const Vec3f & p_position ) : vtx( p_position ) {};

					chemfiles::Vector3D chemfiles;
					Vec3f				vtx;
				};

			  public:
				AtomData() {}
				AtomData( const size_t p_index, const chemfiles::Vector3D & p_position, const float p_distance ) :
					_index( p_index ), _position( p_position ), _distance( p_distance )
				{
				}
				AtomData( const size_t p_index, const Vec3f & p_position, const float p_distance ) :
					_index( p_index ), _position( p_position ), _distance( p_distance )
				{
				}

				size_t						getIndex() { return _index; }
				const chemfiles::Vector3D & getChemfilesPosition() { return _position.chemfiles; }
				const Vec3f &				getVTXPosition() { return _position.vtx; }
				float						getDistance() { return _distance; }

			  private:
				size_t		   _index;
				union position _position = { chemfiles::Vector3D( 0, 0, 0 ) };
				float		   _distance;
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

	  public:
		static void recomputeBondOrders( chemfiles::Frame & p_frame );
		static bool recomputeBondOrdersFromFile( chemfiles::Frame & p_frame );

		static const std::vector<IO::Reader::BondData> & getResidueBonds( const std::string & p_residueSymbol );

	  private:
		// Guess bond order with topology
		static void _buildNeighbourStruct( const chemfiles::Frame &			  p_frame,
										   std::vector<std::vector<size_t>> & p_linkedAtomsVector );

		static void _tagCycles( const chemfiles::Frame &				 p_frame,
								const std::vector<std::vector<size_t>> & p_linkedAtomsVector,
								std::vector<CycleState> &				 p_cycleStatePerAtom );
		static void _tagCyclesRecursive( const chemfiles::Frame &				  p_frame,
										 const std::vector<std::vector<size_t>> & p_linkedAtomsVector,
										 std::vector<CycleState> &				  p_cycleStatePerAtom,
										 std::vector<size_t> &					  p_cycleIndexes,
										 short									  p_counter );

		static void _checkBondOrders( chemfiles::Frame &					   p_frame,
									  const std::vector<std::vector<size_t>> & p_linkedAtomsVector,
									  const std::vector<CycleState> &		   p_cycleStatePerAtom );

		static float _computeAverageCenterDotCross( const chemfiles::Frame &	p_frame,
													const std::vector<size_t> & p_atoms );

		static float _computeAverageRingDotCross( const chemfiles::Frame &	  p_frame,
												  const std::vector<size_t> & p_atoms,
												  const size_t				  p_atomCount,
												  chemfiles::Vector3D &		  dir );

		static bool _verifyPlanarBonds( const chemfiles::Frame &				 p_frame,
										const std::vector<size_t> &				 p_atoms,
										const size_t							 p_atomCount,
										const std::vector<std::vector<size_t>> & p_linkedAtomsVector,
										const chemfiles::Vector3D &				 dir,
										const float								 cutoff );

		static void _normalizeVector( chemfiles::Vector3D & p_vector );

		static void _setBondOrder( chemfiles::Frame &				p_frame,
								   const size_t						p_firstAtomIndex,
								   const size_t						p_secondAtomIndex,
								   const chemfiles::Bond::BondOrder p_bondOrder,
								   const bool						p_force = false );
	};

} // namespace VTX::IO::Util

#endif
