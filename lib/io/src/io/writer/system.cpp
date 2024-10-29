#include "io/writer/system.hpp"
#include "io/writer/chemfiles.hpp"
#include <util/exceptions.hpp>

namespace VTX::IO::Writer
{
	namespace
	{
		inline bool isResidueOfChain(
			const size_t &					  p_residueIdx,
			const size_t &					  p_chainIdx,
			const VTX::Core::Struct::System & p_mol
		) noexcept
		{
			if ( p_chainIdx < p_mol.getChainCount() - 1 )
				return p_residueIdx < p_mol.chainFirstResidues[ p_chainIdx ] + p_mol.chainResidueCounts[ p_chainIdx ];
			return p_residueIdx < p_mol.getResidueCount();
		}
		inline bool isAtomOfResidue(
			const size_t &					  p_atomIdx,
			const size_t &					  p_residueIdx,
			const VTX::Core::Struct::System & p_mol
		)
		{
			if ( p_residueIdx < p_mol.getResidueCount() - 1 )
				return p_atomIdx
					   < p_mol.residueFirstAtomIndexes[ p_residueIdx ] + p_mol.residueAtomCounts[ p_residueIdx ];
			return p_atomIdx < p_mol.getAtomCount();
		}
		void convert( const VTX::Core::ChemDB::Bond::ORDER & in, E_BOND_ORDER & out ) noexcept
		{
			switch ( in )
			{
			case VTX::Core::ChemDB::Bond::ORDER::UNKNOWN: out = E_BOND_ORDER::unknown; break;
			case VTX::Core::ChemDB::Bond::ORDER::SINGLE: out = E_BOND_ORDER::single; break;
			case VTX::Core::ChemDB::Bond::ORDER::DOUBLE: out = E_BOND_ORDER::doubl; break;
			case VTX::Core::ChemDB::Bond::ORDER::TRIPLE: out = E_BOND_ORDER::triple; break;
			case VTX::Core::ChemDB::Bond::ORDER::QUADRUPLE: out = E_BOND_ORDER::quadruple; break;
			case VTX::Core::ChemDB::Bond::ORDER::QUINTUPLET: out = E_BOND_ORDER::quintuple; break;
			case VTX::Core::ChemDB::Bond::ORDER::AROMATIC: out = E_BOND_ORDER::aromatic; break;
			default: out = E_BOND_ORDER::unknown;
			}
		}

		inline void addAtom(
			const VTX::Core::Struct::System & p_mol,
			const size_t &					  p_atomIdx,
			System &						  p_system,
			Residue &						  p_residue
		)
		{
			Atom w_atom = p_system.newAtom( { p_atomIdx } );
			p_residue.add( w_atom );
			w_atom.setName( p_mol.atomNames[ p_atomIdx ] );
			auto & constSymbol
				= VTX::Core::ChemDB::Atom ::SYMBOL_STR[ static_cast<int>( p_mol.atomSymbols[ p_atomIdx ] ) ];
			w_atom.setSymbol( std::string( constSymbol.begin(), constSymbol.end() ) );
		}
		inline void addResidue(
			const VTX::Core::Struct::System & p_mol,
			const size_t &					  p_residueIdx,
			System &						  p_system,
			Chain &							  p_chain,
			AtomFilter &					  p_atomFilter
		)
		{
			Residue w_residue = p_system.newResidue();
			p_chain.add( w_residue );
			w_residue.setResId( static_cast<int>( p_mol.residueOriginalIds[ p_residueIdx ] ) );
			auto & constSymbol
				= VTX::Core::ChemDB::Residue::SYMBOL_STR[ static_cast<int>( p_mol.residueSymbols[ p_residueIdx ] ) ];
			std::string residueSymbol
				= p_mol.residueSymbols[ p_residueIdx ] == VTX::Core::ChemDB::Residue::SYMBOL::UNKNOWN
					  ? p_mol.residueUnknownNames[ p_residueIdx ]
					  : std::string( constSymbol.begin(), constSymbol.end() );
			w_residue.setSymbol( residueSymbol );
			w_residue.set( Property { .key	 = "secondary_structure",
									  .value = VTX::Core::ChemDB::SecondaryStructure::enumToPdbFormatted(
										  p_mol.residueSecondaryStructureTypes[ p_residueIdx ]
									  ) } );
			w_residue.set( Property { .key	 = "is_standard_pdb",
									  .value = VTX::Core::ChemDB::Residue::checkIfStandardFromName( residueSymbol ) } );
			for ( size_t atomIdx = p_mol.residueFirstAtomIndexes[ p_residueIdx ];
				  isAtomOfResidue( atomIdx, p_residueIdx, p_mol );
				  atomIdx++ )
			{
				if ( p_atomFilter( p_mol, atomIdx ) )
					addAtom( p_mol, atomIdx, p_system, w_residue );
			}
		}
		inline void addChain(
			const VTX::Core::Struct::System & p_mol,
			const size_t &					  p_chainIdx,
			System &						  p_system,
			AtomFilter &					  p_atomFilter
		)
		{
			Chain w_chain = p_system.newChain();
			w_chain.setName( p_mol.chainNames[ p_chainIdx ] );

			// When I wrote this, we didn't read the chainID from chemfiles and we don't store it anywhere. So we will
			// put the name instead for now.
			w_chain.setId( p_mol.chainNames[ p_chainIdx ] );

			for ( size_t residueIdx = p_mol.chainFirstResidues[ p_chainIdx ];
				  isResidueOfChain( residueIdx, p_chainIdx, p_mol );
				  residueIdx++ )
			{
				addResidue( p_mol, residueIdx, p_system, w_chain, p_atomFilter );
			}
		}
		inline void setBonds(
			const VTX::Core::Struct::System & p_mol,
			System &						  p_system,
			AtomFilter &					  p_atomFilter

		)
		{
			for ( size_t bondIdx = 0; bondIdx < p_mol.getBondCount(); bondIdx += 2 )
			{
				size_t atomIdx1 = p_mol.bondPairAtomIndexes[ bondIdx ],
					   atomIdx2 = p_mol.bondPairAtomIndexes[ bondIdx + 1 ];
				if ( p_atomFilter( p_mol, atomIdx1 ) && p_atomFilter( p_mol, atomIdx2 ) )
				{
					E_BOND_ORDER w_bondOrder = E_BOND_ORDER::unknown;
					convert( p_mol.bondOrders[ bondIdx >> 1 ], w_bondOrder );
					p_system.bind( { atomIdx1 }, { atomIdx2 }, w_bondOrder );
				}
			}
		}
		inline void fillFrames( const VTX::Core::Struct::System & p_mol, System & p_system )
		{
			for ( size_t frameIdx = 0; frameIdx < p_mol.trajectory.getFrameCount(); frameIdx++ )
			{
				Frame w_frame = p_system.newFrame();
				for ( size_t atomIdx = 0; atomIdx < p_mol.trajectory.frames[ frameIdx ].size(); atomIdx++ )
				{
					Atom w_atom;
					// if the atom doesn't exist for some reason, we skip to the next
					if ( p_system.fetch( w_atom, { atomIdx } ) )
					{
						const VTX::Vec3f & coords = p_mol.trajectory.frames[ frameIdx ][ atomIdx ];
						w_frame.set( w_atom, AtomCoordinates { .x = coords[ 0 ], .y = coords[ 1 ], .z = coords[ 2 ] } );
					}
				}
			}
		}

		void writeTrajectoryFile( WriteArgs p_args )
		{
			ChemfilesTrajectory writer;
			System				w_system = writer.system();

			for ( size_t chainIdx = 0; chainIdx < p_args.molecule->getChainCount(); chainIdx++ )
			{
				addChain( *p_args.molecule, chainIdx, w_system, p_args.atomFilter );
			}

			if ( p_args.stopToken.stop_requested() )
				return;

			setBonds( *p_args.molecule, w_system, p_args.atomFilter );

			if ( p_args.stopToken.stop_requested() )
				return;

			fillFrames( *p_args.molecule, w_system );

			// We fill the write destination at the very end so if we stopped due to the stoptoken, nothing get written
			writer.setWriteDestination( std::move( p_args.destination ) );
			writer.setWriteFormat( std::move( p_args.format ) );
		}
	} // namespace

	void writeFile( WriteArgs p_args )
	{
		if ( p_args.stopToken.stop_requested() )
			return;
		if ( p_args.writeType != E_WRITE_TYPE::trajectory )
			throw VTXException( "Other type of writings aren't implemented yet" );
		if ( p_args.molecule == nullptr )
			throw VTXException( "Molecule was nullptr." );
		writeTrajectoryFile( std::move( p_args ) );
	}
} // namespace VTX::IO::Writer
