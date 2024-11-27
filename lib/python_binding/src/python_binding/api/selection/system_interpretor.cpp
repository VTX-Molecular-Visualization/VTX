#include "python_binding/api/selection/system_interpretor.hpp"
#include "python_binding/api/selection/helper.hpp"
#include <app/application/scene.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/component/chemistry/system.hpp>
#include <app/component/scene/selectable.hpp>
#include <app/helper/chemistry.hpp>
#include <app/vtx_app.hpp>
#include <util/logger.hpp>

namespace VTX::PythonBinding::API::Selection
{
	using namespace App::Component::Chemistry;

	SystemInterpretor::InterpretedKwargs::InterpretedKwargs( const pybind11::kwargs & p_kwargs )
	{
		_hasSystemParams = p_kwargs.contains( "mol_n" ) || p_kwargs.contains( "mol_i" );
		moleculeNames	 = _getStringListInKwargs( p_kwargs, "mol_n" );
		moleculeIndexes	 = _getIndexListInKwargs( p_kwargs, "mol_i" );

		_hasChainParams = p_kwargs.contains( "chain_n" ) || p_kwargs.contains( "chain_i" );
		chainNames		= _getStringListInKwargs( p_kwargs, "chain_n" );
		chainIndexes	= _getIndexListInKwargs( p_kwargs, "chain_i" );

		_hasResidueParams = p_kwargs.contains( "res_n" ) || p_kwargs.contains( "res_i" );
		residueNames	  = _getStringListInKwargs( p_kwargs, "res_n" );
		residueIndexes	  = _getIndexListInKwargs( p_kwargs, "res_i" );

		_hasAtomParams
			= p_kwargs.contains( "atom_n" ) || p_kwargs.contains( "atom_i" ) || p_kwargs.contains( "atom_t" );
		atomNames	= _getStringListInKwargs( p_kwargs, "atom_n" );
		atomIndexes = _getAtomIndexListInKwargs( p_kwargs, "atom_i" );
		atomSymbols = _getEnumListFromStrInKwargs<VTX::Core::ChemDB::Atom::SYMBOL>(
			p_kwargs, "atom_t", &VTX::Core::ChemDB::Atom::getSymbolFromString, VTX::Core::ChemDB::Atom::SYMBOL::UNKNOWN
		);
	}

	std::vector<VTX::Core::ChemDB::Atom::SYMBOL> SystemInterpretor::InterpretedKwargs::_interpretAtomSymbols(
		const pybind11::kwargs & p_kwargs
	)
	{
		std::vector<VTX::Core::ChemDB::Atom::SYMBOL> atomSymbols	= std::vector<VTX::Core::ChemDB::Atom::SYMBOL>();
		const std::vector<std::string>				 atomSymbolsStr = _getStringListInKwargs( p_kwargs, "atom_t" );

		atomSymbols.reserve( atomSymbolsStr.size() );

		for ( const std::string & str : atomSymbolsStr )
		{
			const VTX::Core::ChemDB::Atom::SYMBOL symbol = VTX::Core::ChemDB::Atom::getSymbolFromString( str );

			if ( symbol != VTX::Core::ChemDB::Atom::SYMBOL::UNKNOWN )
			{
				atomSymbols.emplace_back( symbol );
			}
		}

		atomSymbols.shrink_to_fit();

		return atomSymbols;
	}

	bool SystemInterpretor::InterpretedKwargs::isValid() const
	{
		return _hasSystemParams || _hasChainParams || _hasResidueParams || _hasAtomParams;
	}

	bool SystemInterpretor::InterpretedKwargs::hasSpecifySystem() const { return _hasSystemParams; }
	bool SystemInterpretor::InterpretedKwargs::hasSpecifyChain() const { return _hasChainParams; }
	bool SystemInterpretor::InterpretedKwargs::hasSpecifyResidue() const { return _hasResidueParams; }
	bool SystemInterpretor::InterpretedKwargs::hasSpecifyAtom() const { return _hasAtomParams; }

	void SystemInterpretor::interpretSystems(
		App::Application::Selection::Selection & p_selection,
		const pybind11::kwargs &				 p_kwargs
	)
	{
		InterpretedKwargs kwargs = InterpretedKwargs( p_kwargs );

		if ( !kwargs.isValid() )
		{
			return;
		}

		std::set<System *> molecules = _getSystems( kwargs );
		const bool		   selectFullSystem
			= !kwargs.hasSpecifyChain() && !kwargs.hasSpecifyResidue() && !kwargs.hasSpecifyAtom();

		for ( System * const molecule : molecules )
		{
			App::Component::Scene::Selectable & selectableComponent
				= App::ECS_REGISTRY().getComponent<App::Component::Scene::Selectable>( *molecule );

			App::Application::Selection::SystemData & moleculeSelectionData
				= p_selection.select<App::Application::Selection::SystemData>(
					selectableComponent, App::Application::Selection::AssignmentType::APPEND
				);

			if ( !selectFullSystem )
			{
				moleculeSelectionData.clear();
				_selectChains( kwargs, moleculeSelectionData );
			}
		}
	}

	std::set<System *> SystemInterpretor::_getSystems( const InterpretedKwargs & p_kwargs )
	{
		using namespace App;

		std::set<Component::Chemistry::System *> molecules = std::set<Component::Chemistry::System *>();

		if ( p_kwargs.hasSpecifySystem() )
		{
			const std::vector<std::string> & moleculeNames	 = p_kwargs.moleculeNames;
			const std::vector<size_t>		 moleculeIndexes = p_kwargs.moleculeIndexes;

			for ( const std::string & molName : moleculeNames )
			{
				App::Core::ECS::BaseEntity moleculeEntity = App::SCENE().getItem( molName );

				if ( !ECS_REGISTRY().isValid( moleculeEntity ) )
					continue;

				Component::Chemistry::System & moleculeComponent
					= ECS_REGISTRY().getComponent<System>( moleculeEntity );

				molecules.emplace( &moleculeComponent );
			}
			for ( const size_t molIndex : moleculeIndexes )
			{
				App::Core::ECS::BaseEntity moleculeEntity = App::SCENE().getItem( molIndex );

				if ( !ECS_REGISTRY().isValid( moleculeEntity ) )
					continue;

				Component::Chemistry::System & moleculeComponent
					= ECS_REGISTRY().getComponent<System>( moleculeEntity );

				molecules.emplace( &moleculeComponent );
			}
		}
		else
		{
			auto view = App::SCENE().getAllSceneItemsOfType<Component::Chemistry::System>();

			for ( const App::Core::ECS::BaseEntity entity : view )
			{
				Component::Chemistry::System & moleculeComponent = ECS_REGISTRY().getComponent<System>( entity );

				molecules.emplace( &moleculeComponent );
			}
		}

		return molecules;
	}

	void SystemInterpretor::_selectChains(
		const InterpretedKwargs &				  p_kwargs,
		App::Application::Selection::SystemData & p_moleculeSelectionData
	)
	{
		System &   molecule		   = p_moleculeSelectionData.getSystem();
		const bool selectFullChain = !p_kwargs.hasSpecifyResidue() && !p_kwargs.hasSpecifyAtom();

		if ( p_kwargs.hasSpecifyChain() )
		{
			for ( const std::string & chainName : p_kwargs.chainNames )
			{
				std::vector<Chain *> validChains = App::Helper::Chemistry::findChainsByName( molecule, chainName );

				for ( Chain * const chain : validChains )
				{
					if ( selectFullChain )
						p_moleculeSelectionData.selectFullChain( *chain );
					else
						p_moleculeSelectionData.referenceChain( *chain );
				}
			}

			for ( const size_t chainIndex : p_kwargs.chainIndexes )
			{
				Chain * const chain = molecule.getChain( chainIndex );

				if ( selectFullChain )
					p_moleculeSelectionData.selectFullChain( *chain );
				else
					p_moleculeSelectionData.referenceChain( *chain );
			}
		}
		else if ( selectFullChain )
		{
			for ( const std::unique_ptr<Chain> & chain : molecule.getChains() )
			{
				p_moleculeSelectionData.selectFullChain( *chain );
			}
		}

		if ( !selectFullChain )
			_selectResidues( p_kwargs, p_moleculeSelectionData );
	}

	void SystemInterpretor::_selectResidues(
		const InterpretedKwargs &				  p_kwargs,
		App::Application::Selection::SystemData & p_moleculeSelectionData
	)
	{
		const System &													molecule = p_moleculeSelectionData.getSystem();
		const App::Application::Selection::SystemData::IndexRangeList & chainIDs
			= p_moleculeSelectionData.getChainIds();

		const bool selectFullResidue = !p_kwargs.hasSpecifyAtom();

		if ( p_kwargs.hasSpecifyResidue() )
		{
			if ( chainIDs.isEmpty() )
			{
				for ( const std::unique_ptr<Chain> & chain : molecule.getChains() )
				{
					if ( chain == nullptr )
						continue;

					for ( const std::string & residueName : p_kwargs.residueNames )
					{
						const std::vector<Residue *> residues
							= App::Helper::Chemistry::findResiduesByName( *chain, residueName );

						if ( residues.size() > 0 )
						{
							p_moleculeSelectionData.referenceChain( *chain );

							for ( Residue * const residue : residues )
							{
								if ( selectFullResidue )
									p_moleculeSelectionData.selectFullResidue( *residue );
								else
									p_moleculeSelectionData.referenceResidue( *residue );
							}
						}
					}
				}

				for ( const std::unique_ptr<Residue> & residue : molecule.getResidues() )
				{
					for ( const size_t residueIndex : p_kwargs.residueIndexes )
					{
						if ( residue->getIndexInOriginalChain() == residueIndex )
						{
							if ( selectFullResidue )
								p_moleculeSelectionData.selectFullResidue( *residue );
							else
								p_moleculeSelectionData.referenceResidue( *residue );
						}
					}
				}
			}
			else
			{
				for ( const size_t chainID : chainIDs )
				{
					const Chain * chainPtr = molecule.getChain( chainID );
					const Chain & chain	   = *chainPtr;

					for ( const std::string & residueName : p_kwargs.residueNames )
					{
						const std::vector<Residue *> residues
							= App::Helper::Chemistry::findResiduesByName( chain, residueName );

						for ( Residue * const residue : residues )
						{
							if ( selectFullResidue )
								p_moleculeSelectionData.selectFullResidue( *residue );
							else
								p_moleculeSelectionData.referenceResidue( *residue );
						}
					}

					for ( const size_t residueIndex : p_kwargs.residueIndexes )
					{
						for ( const Residue & residue : chain.residues() )
						{
							if ( residue.getIndexInOriginalChain() == residueIndex )
							{
								if ( selectFullResidue )
									p_moleculeSelectionData.selectFullResidue( residue );
								else
									p_moleculeSelectionData.referenceResidue( residue );

								break;
							}
						}
					}
				}
			}
		}

		if ( !selectFullResidue )
		{
			_selectAtoms( p_kwargs, p_moleculeSelectionData );
		}
	}

	void SystemInterpretor::_selectAtoms(
		const InterpretedKwargs &				  p_kwargs,
		App::Application::Selection::SystemData & p_moleculeSelectionData
	)
	{
		System & molecule = p_moleculeSelectionData.getSystem();

		const App::Application::Selection::SystemData::IndexRangeList & residueIDs
			= p_moleculeSelectionData.getResidueIds();

		if ( residueIDs.isEmpty() )
		{
			const App::Application::Selection::SystemData::IndexRangeList & chainIDs
				= p_moleculeSelectionData.getChainIds();

			if ( chainIDs.isEmpty() )
			{
				_addAtomsFollowingKwargs(
					0, atom_index_t( molecule.getAtoms().size() - 1 ), molecule, p_moleculeSelectionData, p_kwargs
				);
			}
			else
			{
				for ( const size_t chainID : chainIDs )
				{
					const Chain * const chain = molecule.getChain( chainID );

					if ( chain == nullptr )
						continue;

					_addAtomsFollowingKwargs(
						chain->getIndexFirstAtom(),
						chain->getIndexLastAtom(),
						molecule,
						p_moleculeSelectionData,
						p_kwargs
					);
				}
			}
		}
		else
		{
			const App::Application::Selection::SystemData::IndexRangeList & residueIDs
				= p_moleculeSelectionData.getResidueIds();

			for ( const size_t residueID : residueIDs )
			{
				const Residue * const residue = molecule.getResidue( residueID );

				if ( residue == nullptr )
					continue;

				_addAtomsFollowingKwargs(
					residue->getIndexFirstAtom(),
					residue->getIndexLastAtom(),
					molecule,
					p_moleculeSelectionData,
					p_kwargs
				);
			}
		}
	}

	void SystemInterpretor::_addAtomsFollowingKwargs(
		const atom_index_t						  p_firstAtom,
		const atom_index_t						  p_lastAtom,
		System &								  p_molecule,
		App::Application::Selection::SystemData & p_moleculeSelectionData,
		const InterpretedKwargs &				  p_kwargs
	)
	{
		for ( atom_index_t atomID = p_firstAtom; atomID <= p_lastAtom; atomID++ )
		{
			Atom * const atom = p_molecule.getAtom( atomID );

			if ( atom == nullptr )
				continue;

			for ( const std::string & atomName : p_kwargs.atomNames )
			{
				if ( atom->getName() == atomName )
				{
					p_moleculeSelectionData.selectAtom( *atom );
					break;
				}
			}

			for ( const VTX::Core::ChemDB::Atom::SYMBOL atomSymbol : p_kwargs.atomSymbols )
			{
				if ( atom->getSymbol() == atomSymbol )
				{
					p_moleculeSelectionData.selectAtom( *atom );
					break;
				}
			}
		}
	}

} // namespace VTX::PythonBinding::API::Selection
