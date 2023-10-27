#include "python_binding/api/selection/molecule_interpretor.hpp"
#include "python_binding/api/selection/helper.hpp"
#include <app/application/scene.hpp>
#include <app/application/selection/molecule_data.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/component/scene/selectable.hpp>
#include <app/helper/chemistry.hpp>
#include <app/vtx_app.hpp>

namespace VTX::PythonBinding::API::Selection
{
	void MoleculeInterpretor::interpretMolecules(
		App::Application::Selection::Selection & p_selection,
		const pybind11::kwargs &				 kwargs
	)
	{
		using MoleculeComponent = App::Component::Chemistry::Molecule;

		if ( !_hasMoleculeOptionsInKwargs( kwargs ) )
		{
			return;
		}

		std::set<App::Component::Chemistry::Molecule *> molecules = _getMolecules( kwargs );

		const std::vector<std::string> chainNames	   = _getStringListInKwargs( kwargs, "chain_n" );
		const std::vector<size_t>	   chainIndexes	   = _getIndexListInKwargs( kwargs, "chain_i" );
		const bool					   hasSpecifyChain = chainNames.size() + chainIndexes.size() > 0;

		const std::vector<std::string> residueNames		 = _getStringListInKwargs( kwargs, "res_n" );
		const std::vector<size_t>	   residueIndexes	 = _getIndexListInKwargs( kwargs, "res_i" );
		const bool					   hasSpecifyResidue = residueNames.size() + residueIndexes.size() > 0;

		const std::vector<std::string> atomNames	  = _getStringListInKwargs( kwargs, "atom_n" );
		const std::vector<size_t>	   atomIndexes	  = _getIndexListInKwargs( kwargs, "atom_i" );
		const bool					   hasSpecifyAtom = atomNames.size() + atomIndexes.size() > 0;

		for ( MoleculeComponent * const molecule : molecules )
		{
			App::Component::Scene::Selectable & selectableComponent
				= App::VTXApp::get().MAIN_REGISTRY().getComponent<App::Component::Scene::Selectable>( *molecule );

			App::Application::Selection::MoleculeData & moleculeSelectionData
				= dynamic_cast<App::Application::Selection::MoleculeData &>( p_selection.select( selectableComponent )
				);

			if ( !hasSpecifyChain && !hasSpecifyResidue && !hasSpecifyAtom )
			{
				moleculeSelectionData.selectAll();
			}
			else
			{
				std::set<App::Component::Chemistry::Chain *> chains = std::set<App::Component::Chemistry::Chain *>();

				if ( hasSpecifyChain )
				{
					for ( const std::string & chainName : chainNames )
					{
						std::vector<App::Component::Chemistry::Chain *> validChains
							= App::Helper::Chemistry::findChainsByName( *molecule, chainName );

						for ( App::Component::Chemistry::Chain * const chain : validChains )
							chains.emplace( chain );
					}

					for ( const size_t chainIndex : chainIndexes )
					{
						App::Component::Chemistry::Chain * const chain = molecule->getChain( chainIndex );
						if ( chain != nullptr )
							chains.emplace( chain );
					}
				}
				else
				{
					for ( App::Component::Chemistry::Chain * chain : molecule->getChains() )
						if ( chain != nullptr )
							chains.emplace( chain );
				}
			}
		}
	}

	std::set<App::Component::Chemistry::Molecule *> MoleculeInterpretor::_getMolecules( const pybind11::kwargs & kwargs
	)
	{
		using namespace App;

		const std::vector<std::string> moleculeNames   = _getStringListInKwargs( kwargs, "mol_n" );
		const std::vector<size_t>	   moleculeIndexes = _getIndexListInKwargs( kwargs, "mol_i" );

		std::set<Component::Chemistry::Molecule *> molecules = std::set<Component::Chemistry::Molecule *>();

		for ( const std::string & molName : moleculeNames )
		{
			App::Core::ECS::BaseEntity moleculeEntity = App::VTXApp::get().getScene().getItem( molName );

			if ( !VTXApp::get().MAIN_REGISTRY().isValid( moleculeEntity ) )
				continue;

			Component::Chemistry::Molecule & moleculeComponent
				= VTXApp::get().MAIN_REGISTRY().getComponent<App::Component::Chemistry::Molecule>( moleculeEntity );

			molecules.emplace( &moleculeComponent );
		}
		for ( const size_t molIndex : moleculeIndexes )
		{
			App::Core::ECS::BaseEntity moleculeEntity = App::VTXApp::get().getScene().getItem( molIndex );

			if ( !VTXApp::get().MAIN_REGISTRY().isValid( moleculeEntity ) )
				continue;

			Component::Chemistry::Molecule & moleculeComponent
				= VTXApp::get().MAIN_REGISTRY().getComponent<App::Component::Chemistry::Molecule>( moleculeEntity );

			molecules.emplace( &moleculeComponent );
		}

		// If no molecule selected => select all molecules
		if ( molecules.size() == 0 )
		{
			auto view = App::VTXApp::get().getScene().getAllSceneItemsOfType<Component::Chemistry::Molecule>();

			for ( const App::Core::ECS::BaseEntity entity : view )
			{
				Component::Chemistry::Molecule & moleculeComponent
					= VTXApp::get().MAIN_REGISTRY().getComponent<App::Component::Chemistry::Molecule>( entity );

				molecules.emplace( &moleculeComponent );
			}
		}

		return molecules;
	}

	bool MoleculeInterpretor::_hasMoleculeOptionsInKwargs( const pybind11::kwargs & p_kwargs )
	{
		return p_kwargs.contains( "mol_n" ) || p_kwargs.contains( "mol_i" ) || p_kwargs.contains( "chain_n" )
			   || p_kwargs.contains( "chain_i" ) || p_kwargs.contains( "res_n" ) || p_kwargs.contains( "res_i" )
			   || p_kwargs.contains( "atom_n" ) || p_kwargs.contains( "atom_i" );
	}

} // namespace VTX::PythonBinding::API::Selection
