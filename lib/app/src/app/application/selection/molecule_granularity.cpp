#include "app/application/selection/molecule_granularity.hpp"
#include "app/action/selection.hpp"
#include "app/application/selection/selection_manager.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/component/scene/selectable.hpp"
#include "app/core/action/action_system.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Application::Selection
{
	MoleculeData MoleculeGranularity::getSelectionData( const Atom & p_atom, const Granularity & p_granularity )
	{
		const auto & molSelectable
			= ECS_REGISTRY().getComponent<Component::Scene::Selectable>( *p_atom.getMoleculePtr() );
		auto molData = MoleculeData( molSelectable );
		molData.clear();

		switch ( p_granularity )
		{
		case Granularity::MOLECULE: molData.selectAll(); break;
		case Granularity::CHAIN: molData.selectFullChain( *p_atom.getConstChainPtr() ); break;
		case Granularity::RESIDUE: molData.selectFullResidue( *p_atom.getConstResiduePtr() ); break;
		case Granularity::ATOM:
		default: molData.selectAtom( p_atom ); break;
		}

		return molData;
	}
	MoleculeData MoleculeGranularity::getSelectionData( const Residue & p_residue, const Granularity & p_granularity )
	{
		const auto & molSelectable
			= ECS_REGISTRY().getComponent<Component::Scene::Selectable>( *p_residue.getMoleculePtr() );
		MoleculeData molData = MoleculeData( molSelectable );
		molData.clear();

		switch ( p_granularity )
		{
		case Granularity::MOLECULE: molData.selectAll(); break;
		case Granularity::CHAIN: molData.selectFullChain( *p_residue.getConstChainPtr() ); break;
		case Granularity::RESIDUE:
		case Granularity::ATOM:
		default: molData.selectFullResidue( p_residue ); break;
		}

		return molData;
	}
	MoleculeData MoleculeGranularity::getSelectionData( const Chain & p_chain, const Granularity & p_granularity )
	{
		const auto & molSelectable
			= ECS_REGISTRY().getComponent<Component::Scene::Selectable>( *p_chain.getMoleculePtr() );
		MoleculeData molData = MoleculeData( molSelectable );
		molData.clear();

		switch ( p_granularity )
		{
		case Granularity::MOLECULE: molData.selectAll(); break;
		case Granularity::CHAIN:
		case Granularity::RESIDUE:
		case Granularity::ATOM:
		default: molData.selectFullChain( p_chain ); break;
		}

		return molData;
	}

	void MoleculeGranularity::select(
		const Atom &		 p_atom,
		const Granularity &	 p_granularity,
		const AssignmentType p_assignment
	)
	{
		const auto & molSelectable
			= ECS_REGISTRY().getComponent<Component::Scene::Selectable>( *p_atom.getMoleculePtr() );

		const MoleculeData molData = getSelectionData( p_atom, p_granularity );

		ACTION_SYSTEM().execute<App::Action::Selection::Select>( molData, p_assignment );
	}
	void MoleculeGranularity::unselect( const Atom & p_atom, const Granularity & p_granularity )
	{
		const auto & molSelectable
			= ECS_REGISTRY().getComponent<Component::Scene::Selectable>( *p_atom.getMoleculePtr() );

		const MoleculeData molData = getSelectionData( p_atom, p_granularity );

		ACTION_SYSTEM().execute<App::Action::Selection::Unselect>( molData );
	}

	void MoleculeGranularity::select(
		const Residue &		 p_residue,
		const Granularity &	 p_granularity,
		const AssignmentType p_assignment
	)
	{
		const auto & molSelectable
			= ECS_REGISTRY().getComponent<Component::Scene::Selectable>( *p_residue.getMoleculePtr() );

		const MoleculeData molData = getSelectionData( p_residue, p_granularity );

		ACTION_SYSTEM().execute<App::Action::Selection::Select>( molData, p_assignment );
	}
	void MoleculeGranularity::unselect( const Residue & p_residue, const Granularity & p_granularity )
	{
		const auto & molSelectable
			= ECS_REGISTRY().getComponent<Component::Scene::Selectable>( *p_residue.getMoleculePtr() );

		const MoleculeData molData = getSelectionData( p_residue, p_granularity );

		ACTION_SYSTEM().execute<App::Action::Selection::Unselect>( molData );
	}

	void MoleculeGranularity::select(
		const Chain &		 p_chain,
		const Granularity &	 p_granularity,
		const AssignmentType p_assignment
	)
	{
		const auto & molSelectable
			= ECS_REGISTRY().getComponent<Component::Scene::Selectable>( *p_chain.getMoleculePtr() );

		const MoleculeData molData = getSelectionData( p_chain, p_granularity );

		ACTION_SYSTEM().execute<App::Action::Selection::Select>( molData, p_assignment );
	}
	void MoleculeGranularity::unselect( const Chain & p_chain, const Granularity & p_granularity )
	{
		const auto & molSelectable
			= ECS_REGISTRY().getComponent<Component::Scene::Selectable>( *p_chain.getMoleculePtr() );

		const MoleculeData molData = getSelectionData( p_chain, p_granularity );

		ACTION_SYSTEM().execute<App::Action::Selection::Unselect>( molData );
	}
} // namespace VTX::App::Application::Selection
