#include "app/component/chemistry/molecule.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/application/system/uid_system.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/bond.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/component/scene/scene_item_component.hpp"
#include "app/vtx_app.hpp"
#include <algorithm>
#include <util/constants.hpp>

namespace VTX::App::Component::Chemistry
{
	namespace ChemDB = VTX::Core::ChemDB;

	Molecule::Molecule()
	{
		_chains	  = std::vector<std::unique_ptr<Chemistry::Chain>>();
		_residues = std::vector<std::unique_ptr<Chemistry::Residue>>();
		_atoms	  = std::vector<std::unique_ptr<Chemistry::Atom>>();
		_bonds	  = std::vector<std::unique_ptr<Chemistry::Bond>>();
	};
	Molecule::Molecule( VTX::Core::Struct::Molecule & p_moleculeStruct ) { setMoleculeStruct( p_moleculeStruct ); }
	Molecule::~Molecule()
	{
		if ( _atomUidRange.isValid() )
			UID_SYSTEM().unregister( _atomUidRange );

		if ( _residueUidRange.isValid() )
			UID_SYSTEM().unregister( _residueUidRange );
	};

	void Molecule::setMoleculeStruct( VTX::Core::Struct::Molecule & p_moleculeStruct )
	{
		_moleculeStruct = std::move( p_moleculeStruct );

		initChains( _moleculeStruct.getChainCount() );
		initResidues( _moleculeStruct.getResidueCount() );
		initAtoms( _moleculeStruct.getAtomCount() );
		initBonds( _moleculeStruct.getBondCount() );
	}

	void Molecule::initChains( const size_t p_chainCount )
	{
		_chains.resize( p_chainCount );
		std::generate(
			_chains.begin(),
			_chains.end(),
			[ this, n = 0 ]() mutable { return std::move( std::make_unique<Chain>( this, n++ ) ); }
		);
	}

	void Molecule::initResidues( const size_t p_residueCount )
	{
		_residues.resize( p_residueCount );
		std::generate(
			_residues.begin(),
			_residues.end(),
			[ this, n = 0 ]() mutable { return std::move( std::make_unique<Residue>( this, n++ ) ); }
		);
	}

	void Molecule::initAtoms( const size_t p_atomCount )
	{
		_atoms.resize( p_atomCount );
		std::generate(
			_atoms.begin(),
			_atoms.end(),
			[ this, n = 0 ]() mutable { return std::move( std::make_unique<Atom>( this, n++ ) ); }
		);

		_atomVisibilities.resize( p_atomCount, uint( true ) );

		_atomColors.resize( p_atomCount, 0 );

		_atomRadii.resize( p_atomCount, 0.f );
		std::generate(
			_atomRadii.begin(),
			_atomRadii.end(),
			[ this, i = 0 ]() mutable
			{ return ChemDB::Atom::SYMBOL_VDW_RADIUS[ int( _moleculeStruct.atomSymbols[ i ] ) ]; }
		);

		_atomUidRange = UID_SYSTEM().registerRange( Core::UID::uid( p_atomCount ) );

		const uint offset = uint( _atomUidRange.getFirst() );

		_atomIds.resize( p_atomCount, Core::UID::INVALID_UID );
		std::generate( _atomIds.begin(), _atomIds.end(), [ this, offset, i = 0 ]() mutable { return offset + i++; } );

		_atomSelections.resize( p_atomCount, uint( false ) );
	}

	void Molecule::initBonds( const size_t p_bondCount )
	{
		_bonds.resize( p_bondCount );
		std::generate(
			_bonds.begin(),
			_bonds.end(),
			[ this, n = 0 ]() mutable { return std::move( std::make_unique<Bond>( this, n++ ) ); }
		);
	}

	void Molecule::setName( const std::string & p_name )
	{
		Component::Scene::SceneItemComponent & sceneComponent
			= MAIN_REGISTRY().getComponent<Component::Scene::SceneItemComponent>( *this );
		sceneComponent.setName( p_name );
		_moleculeStruct.name = p_name;
	}

	const Atom * Molecule::getAtomFromUID( Core::UID::uid p_uid ) const
	{
		return getAtom( p_uid - _atomUidRange.getFirst() );
	}
	Atom * Molecule::getAtomFromUID( Core::UID::uid p_uid ) { return getAtom( p_uid - _atomUidRange.getFirst() ); }
	const Residue * Molecule::getResidueFromUID( Core::UID::uid p_uid ) const
	{
		return getResidue( p_uid - _residueUidRange.getFirst() );
	}
	Residue * Molecule::getResidueFromUID( Core::UID::uid p_uid )
	{
		return getResidue( p_uid - _residueUidRange.getFirst() );
	}

} // namespace VTX::App::Component::Chemistry
