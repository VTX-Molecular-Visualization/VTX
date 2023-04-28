#include "app/internal/io/reader/lib_mmtf.hpp"
#include "app/old_app/color/rgba.hpp"
#include <magic_enum.hpp>
#include <util/types.hpp>

namespace VTX::App::Internal::IO::Reader
{
	/*
	void LibMMTF::readFile( const Path & p_path, App::Component::Chemistry::Molecule & p_molecule )
	{
		mmtf::StructureData data;
		mmtf::decodeFromFile( data, p_path.c_str() );
		return _readStructureData( data, p_molecule );
	}

	void LibMMTF::readBuffer( const std::string & p_buffer,
							  const std::string & p_extension,
							  App::Component::Chemistry::Molecule &	  p_molecule )
	{
		mmtf::StructureData data;
		mmtf::decodeFromBuffer( data, p_buffer.c_str(), p_buffer.size() );

		return _readStructureData( data, p_molecule );
	}

	void LibMMTF::_readStructureData( const mmtf::StructureData & p_data, App::Component::Chemistry::Molecule &
p_molecule )
	{
		// Check for consistency.
		if ( p_data.hasConsistentData( true ) == false )
		{
			throw IOException( "File is not consistent" );
		}

		// Set molecule properties.
		p_molecule.setName( p_data.title );
		p_molecule.setColor( Color::Rgb::randomPastel() );

		float x, y, z;
		uint  chainGlobalIdx   = 0;
		uint  residueGlobalIdx = 0;
		uint  atomGlobalIdx	   = 0;
		uint  bondGlobalIdx	   = 0;

		p_molecule.addAtomPositionFrame();
		App::Component::Chemistry::Molecule::AtomPositionsFrame & frame = p_molecule.getAtomPositionFrame( 0 );

		uint chainCount = p_data.chainsPerModel[ 0 ];
#ifdef _DEBUG
		p_molecule.chainCount = chainCount;
#endif
		// For each chain in the model 0.
		for ( ; chainGlobalIdx < chainCount; ++chainGlobalIdx )
		{
			// New chain.
			p_molecule.addChain();
			App::Component::Chemistry::Chain & chain = p_molecule.getChain( chainGlobalIdx );
			chain.setMoleculePtr( &p_molecule );
			chain.setIndex( chainGlobalIdx );
			chain.setName( p_data.chainNameList[ chainGlobalIdx ] );
			chain.setIdFirstResidue( residueGlobalIdx );
			chain.setResidueCount( p_data.groupsPerChain[ chainGlobalIdx ] );
			chain.setColor( App::Component::Chemistry::Chain::getChainIdColor( p_data.chainIdList[
chainGlobalIdx ] ) );

			// For each residue in the chain.
			uint residueCount = p_data.groupsPerChain[ chainGlobalIdx ];
			if ( residueCount == 0 )
			{
				VTX_WARNING( "No residues" );
			}
#ifdef _DEBUG
			p_molecule.residueCount += residueCount;
#endif
			for ( uint residueLocalIdx = 0; residueLocalIdx < residueCount;
				  ++residueLocalIdx, ++residueGlobalIdx )
			{
				const mmtf::GroupType & group = p_data.groupList[ p_data.groupTypeList[ residueGlobalIdx ] ];

				// New residue.
				p_molecule.addResidue();
				App::Component::Chemistry::Residue & residue = p_molecule.getResidue( residueGlobalIdx );
				residue.setChainPtr( &chain );
				residue.setIndex( residueGlobalIdx );
				const std::string & residueSymbol = group.groupName;
				std::optional		symbol = magic_enum::enum_cast<App::Internal::ChemDB::Residue::SYMBOL>(
residueSymbol ); symbol.has_value() ? residue.setSymbol( symbol.value() ) : p_molecule.addUnknownResidueSymbol(
residueSymbol ); residue.setColor( App::Component::Chemistry::Residue::SYMBOL_COLOR[ int( residue.getSymbol() ) ] );
				residue.setIdFirstAtom( atomGlobalIdx );
				residue.setAtomCount( uint( group.atomNameList.size() ) );
				// residue.setIdFirstBond( bondGlobalIdx );
				// residue.setBondCount( uint( group.bondAtomList.size() ) / 2u ); // 2 index by bond.
				if ( group.bondAtomList.size() % 2 != 0 )
				{
					VTX_WARNING( "Incorrect number of bonds" );
				}

				// For each atom in the residue.
				uint atomCount = uint( group.atomNameList.size() );
				if ( atomCount == 0 )
				{
					VTX_WARNING( "No atoms" );
				}
#ifdef _DEBUG
				p_molecule.atomCount += atomCount;
#endif

				for ( uint atomIdx = 0; atomIdx < atomCount; ++atomIdx, ++atomGlobalIdx )
				{
					// New atom.
					p_molecule.addAtom();
					App::Component::Chemistry::Atom & atom = p_molecule.getAtom( atomGlobalIdx );
					atom.setResiduePtr( &residue );
					atom.setIndex( atomGlobalIdx );
					const std::string & atomSymbol = group.elementList[ atomIdx ];
					std::optional symbol = magic_enum::enum_cast<App::Component::Chemistry::Atom::SYMBOL>( "A_"
+ atomSymbol ); symbol.has_value() ? atom.setSymbol( symbol.value() ) : p_molecule.addUnknownAtomSymbol( atomSymbol );

					atom.setName( group.atomNameList[ atomIdx ] );
					atom.setColor( App::Component::Chemistry::Atom::SYMBOL_COLOR[ (int)atom.getSymbol() ] );

					x = p_data.xCoordList[ atomGlobalIdx ];
					y = p_data.yCoordList[ atomGlobalIdx ];
					z = p_data.zCoordList[ atomGlobalIdx ];

					frame.emplace_back( x, y, z );
					const Vec3f & atomPosition = frame[ atomGlobalIdx ];
					const float	  atomRadius   = atom.getVdwRadius();
				}

				// For each bond in the residue.
				uint bondCount = uint( group.bondAtomList.size() ) / 2u;
#ifdef _DEBUG
				p_molecule.bondCount += bondCount;
#endif
				for ( uint boundIdx = 0; boundIdx < bondCount * 2; boundIdx += 2, bondGlobalIdx++ )
				{
					p_molecule.addBond();
					App::Component::Chemistry::Bond & bond = p_molecule.getBond( bondGlobalIdx );

					bond.setIndexFirstAtom( residue.getIdFirstAtom() + group.bondAtomList[ boundIdx ] );
					bond.setIndexSecondAtom( residue.getIdFirstAtom() + group.bondAtomList[ boundIdx + 1 ] );
				}
			}
		}

		// Add all atom's bonds.
		uint bondCount = uint( p_data.bondAtomList.size() ) / 2u;
		for ( uint boundIdx = 0; boundIdx < bondCount * 2; boundIdx += 2, ++bondGlobalIdx )
		{
			p_molecule.addBond();
			App::Component::Chemistry::Bond & bond = p_molecule.getBond( bondGlobalIdx );

			bond.setIndexFirstAtom( p_data.bondAtomList[ boundIdx ] );
			bond.setIndexSecondAtom( p_data.bondAtomList[ boundIdx + 1 ] );
		}

#ifdef _DEBUG
		p_molecule.bondCount += (uint)p_data.bondAtomList.size() / 2u;
#endif

#ifdef _DEBUG
		if ( p_molecule.getChainCount() != p_molecule.chainCount
			 || p_molecule.getChainCount() != p_data.numChains )
		{
			VTX_ERROR( "Chain count error: " + std::to_string( p_molecule.getChainCount() ) + " / "
					   + std::to_string( p_molecule.chainCount ) + " / " + std::to_string( p_data.numChains ) );
		}
		if ( p_molecule.getResidueCount() != p_molecule.residueCount
			 || p_molecule.getResidueCount() != p_data.numGroups )
		{
			VTX_ERROR( "Residue count error: " + std::to_string( p_molecule.getResidueCount() ) + " / "
					   + std::to_string( p_molecule.residueCount ) + " / "
					   + std::to_string( p_data.numGroups ) );
		}
		if ( p_molecule.getAtomCount() != p_molecule.atomCount || p_molecule.getAtomCount() != p_data.numAtoms )
		{
			VTX_ERROR( "Atom count error: " + std::to_string( p_molecule.getAtomCount() ) + " / "
					   + std::to_string( p_molecule.atomCount ) + " / " + std::to_string( p_data.numAtoms ) );
		}
		if ( p_molecule.getBondCount() != p_molecule.bondCount || p_molecule.getBondCount() != p_data.numBonds )
		{
			VTX_ERROR( "Bond count error: " + std::to_string( p_molecule.getBondCount() ) + " / "
					   + std::to_string( p_molecule.bondCount ) + " / " + std::to_string( p_data.numBonds ) );
		}
#endif
	}
	*/
} // namespace VTX::App::Internal::IO::Reader
