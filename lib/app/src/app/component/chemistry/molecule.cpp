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
#include <util/algorithm/range.hpp>
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

		_atomUidRange = UID_SYSTEM().registerRange( Core::UID::uid( p_atomCount ) );
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

	bool Molecule::isVisible() const { return !_visibleAtomIds.isEmpty(); }
	bool Molecule::isFullyVisible() const { return _visibleAtomIds.count() == atom_index_t( _atoms.size() ); }

	void Molecule::setVisible( const bool p_visible )
	{
		const AtomIndexRange atomRange = AtomIndexRange( 0, atom_index_t( _atoms.size() ) );

		if ( p_visible )
		{
			_visibleAtomIds.addRange( atomRange );
		}
		else
		{
			// = _visibleAtomIds.removeRange( atomRange ), but quicker
			_visibleAtomIds.clear();
		}

		const App::Core::VISIBILITY_APPLY_MODE applyMode
			= p_visible ? App::Core::VISIBILITY_APPLY_MODE::SHOW : App::Core::VISIBILITY_APPLY_MODE::HIDE;
		onVisibilityChange( AtomIndexRangeList( { atomRange } ), applyMode );
	}
	void Molecule::setVisible( const atom_index_t & p_atomId, bool p_visible )
	{
		App::Core::VISIBILITY_APPLY_MODE applyMode;

		if ( p_visible )
		{
			_visibleAtomIds.addValue( p_atomId );
			applyMode = App::Core::VISIBILITY_APPLY_MODE::SHOW;
		}
		else
		{
			_visibleAtomIds.removeValue( p_atomId );
			applyMode = App::Core::VISIBILITY_APPLY_MODE::HIDE;
		}

		onVisibilityChange( AtomIndexRangeList( { AtomIndexRange( p_atomId ) } ), applyMode );
	}
	void Molecule::setVisible( const AtomIndexRange & p_atomRange, bool p_visible )
	{
		App::Core::VISIBILITY_APPLY_MODE applyMode;

		if ( p_visible )
		{
			_visibleAtomIds.addRange( p_atomRange );
			applyMode = App::Core::VISIBILITY_APPLY_MODE::SHOW;
		}
		else
		{
			_visibleAtomIds.removeRange( p_atomRange );
			applyMode = App::Core::VISIBILITY_APPLY_MODE::HIDE;
		}

		onVisibilityChange( AtomIndexRangeList( { p_atomRange } ), applyMode );
	}

	void Molecule::setVisible( const AtomIndexRangeList & p_atomRange, bool p_visible )
	{
		App::Core::VISIBILITY_APPLY_MODE applyMode;

		if ( p_visible )
		{
			Util::Algorithm::Range::mergeInSitu( _visibleAtomIds, p_atomRange );
			applyMode = App::Core::VISIBILITY_APPLY_MODE::SHOW;
		}
		else
		{
			Util::Algorithm::Range::substractInSitu( _visibleAtomIds, p_atomRange );
			applyMode = App::Core::VISIBILITY_APPLY_MODE::HIDE;
		}

		onVisibilityChange( p_atomRange, applyMode );
	}

	void Molecule::setAtomVisibilities( const AtomIndexRangeList & p_visibility )
	{
		_visibleAtomIds = p_visibility;
		onVisibilityChange( _visibleAtomIds, App::Core::VISIBILITY_APPLY_MODE::SET );
	}

} // namespace VTX::App::Component::Chemistry
