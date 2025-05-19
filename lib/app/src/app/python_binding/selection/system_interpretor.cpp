#include "app/python_binding/selection/system_interpretor.hpp"
#include "app/python_binding/selection/helper.hpp"
#include <app/application/scene.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/component/chemistry/system.hpp>
#include <app/component/scene/selectable.hpp>
#include <app/helper/chemistry.hpp>
#include <app/vtx_app.hpp>
#include <util/logger.hpp>

namespace VTX::App::PythonBinding::Selection
{
	using namespace App::Component::Chemistry;

	SystemInterpretor::InterpretedKwargs::InterpretedKwargs( const PythonKwargs & p_kwargs )
	{
		_hasSystemParams = p_kwargs.contains( "system_names" ) || p_kwargs.contains( "mol_i" );
		p_kwargs.get( "system_names", systemNames );
		p_kwargs.get( "mol_i", systemIndexes );

		_hasChainParams = p_kwargs.contains( "chain_n" ) || p_kwargs.contains( "chain_i" );
		p_kwargs.get( "chain_n", chainNames );
		p_kwargs.get( "chain_i", chainIndexes );

		_hasResidueParams = p_kwargs.contains( "res_n" ) || p_kwargs.contains( "res_i" );
		p_kwargs.get( "res_n", residueNames );
		p_kwargs.get( "res_i", residueIndexes );

		_hasAtomParams
			= p_kwargs.contains( "atom_n" ) || p_kwargs.contains( "atom_i" ) || p_kwargs.contains( "atom_t" );
		p_kwargs.get( "atom_n", atomNames );
		p_kwargs.get( "atom_i", atomIndexes );
		p_kwargs.get( "atom_t", &VTX::Core::ChemDB::Atom::getSymbolFromString, atomSymbols );
	}

	std::vector<VTX::Core::ChemDB::Atom::SYMBOL> SystemInterpretor::InterpretedKwargs::_interpretAtomSymbols(
		const PythonKwargs & p_kwargs
	)
	{
		std::vector<VTX::Core::ChemDB::Atom::SYMBOL> atomSymbols = std::vector<VTX::Core::ChemDB::Atom::SYMBOL>();
		std::vector<std::string>					 atomSymbolsStr;
		p_kwargs.get( "atom_t", atomSymbolsStr );

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

	void SystemInterpretor::interpretSystems( App::Selection::Selection & p_selection, const PythonKwargs & p_kwargs )
	{
		InterpretedKwargs kwargs = InterpretedKwargs( p_kwargs );

		if ( !kwargs.isValid() )
		{
			return;
		}

		std::set<System *> systems = _getSystems( kwargs );
		const bool		   selectFullSystem
			= !kwargs.hasSpecifyChain() && !kwargs.hasSpecifyResidue() && !kwargs.hasSpecifyAtom();

		for ( System * const system : systems )
		{
			App::Component::Scene::Selectable & selectableComponent
				= App::ECS_REGISTRY().getComponent<App::Component::Scene::Selectable>( *system );

			App::Selection::SystemData & systemSelectionData = p_selection.select<App::Selection::SystemData>(
				selectableComponent, App::Selection::AssignmentType::APPEND
			);

			if ( !selectFullSystem )
			{
				systemSelectionData.clear();
				_selectChains( kwargs, systemSelectionData );
			}
		}
	}

	std::set<System *> SystemInterpretor::_getSystems( const InterpretedKwargs & p_kwargs )
	{
		using namespace App;

		std::set<Component::Chemistry::System *> systems = std::set<Component::Chemistry::System *>();

		if ( p_kwargs.hasSpecifySystem() )
		{
			const std::vector<std::string> & systemNames   = p_kwargs.systemNames;
			const std::vector<size_t>		 systemIndexes = p_kwargs.systemIndexes;

			for ( const std::string & molName : systemNames )
			{
				App::Core::ECS::BaseEntity systemEntity = App::SCENE().getItem( molName );

				if ( !ECS_REGISTRY().isValid( systemEntity ) )
					continue;

				Component::Chemistry::System & systemComponent = ECS_REGISTRY().getComponent<System>( systemEntity );

				systems.emplace( &systemComponent );
			}
			for ( const size_t molIndex : systemIndexes )
			{
				App::Core::ECS::BaseEntity systemEntity = App::SCENE().getItem( molIndex );

				if ( !ECS_REGISTRY().isValid( systemEntity ) )
					continue;

				Component::Chemistry::System & systemComponent = ECS_REGISTRY().getComponent<System>( systemEntity );

				systems.emplace( &systemComponent );
			}
		}
		else
		{
			auto view = App::SCENE().getAllSceneItemsOfType<Component::Chemistry::System>();

			for ( const App::Core::ECS::BaseEntity entity : view )
			{
				Component::Chemistry::System & systemComponent = ECS_REGISTRY().getComponent<System>( entity );

				systems.emplace( &systemComponent );
			}
		}

		return systems;
	}

	void SystemInterpretor::_selectChains(
		const InterpretedKwargs &	 p_kwargs,
		App::Selection::SystemData & p_systemSelectionData
	)
	{
		System &   system		   = p_systemSelectionData.getSystem();
		const bool selectFullChain = !p_kwargs.hasSpecifyResidue() && !p_kwargs.hasSpecifyAtom();

		if ( p_kwargs.hasSpecifyChain() )
		{
			for ( const std::string & chainName : p_kwargs.chainNames )
			{
				std::vector<Chain *> validChains = App::Helper::Chemistry::findChainsByName( system, chainName );

				for ( Chain * const chain : validChains )
				{
					if ( selectFullChain )
						p_systemSelectionData.selectFullChain( *chain );
					else
						p_systemSelectionData.referenceChain( *chain );
				}
			}

			for ( const size_t chainIndex : p_kwargs.chainIndexes )
			{
				Chain * const chain = system.getChain( chainIndex );

				if ( selectFullChain )
					p_systemSelectionData.selectFullChain( *chain );
				else
					p_systemSelectionData.referenceChain( *chain );
			}
		}
		else if ( selectFullChain )
		{
			for ( const std::unique_ptr<Chain> & chain : system.getChains() )
			{
				p_systemSelectionData.selectFullChain( *chain );
			}
		}

		if ( !selectFullChain )
			_selectResidues( p_kwargs, p_systemSelectionData );
	}

	void SystemInterpretor::_selectResidues(
		const InterpretedKwargs &	 p_kwargs,
		App::Selection::SystemData & p_systemSelectionData
	)
	{
		const System &									   system	= p_systemSelectionData.getSystem();
		const App::Selection::SystemData::IndexRangeList & chainIDs = p_systemSelectionData.getChainIds();

		const bool selectFullResidue = !p_kwargs.hasSpecifyAtom();

		if ( p_kwargs.hasSpecifyResidue() )
		{
			if ( chainIDs.isEmpty() )
			{
				for ( const std::unique_ptr<Chain> & chain : system.getChains() )
				{
					if ( chain == nullptr )
						continue;

					for ( const std::string & residueName : p_kwargs.residueNames )
					{
						const std::vector<Residue *> residues
							= App::Helper::Chemistry::findResiduesByName( *chain, residueName );

						if ( residues.size() > 0 )
						{
							p_systemSelectionData.referenceChain( *chain );

							for ( Residue * const residue : residues )
							{
								if ( selectFullResidue )
									p_systemSelectionData.selectFullResidue( *residue );
								else
									p_systemSelectionData.referenceResidue( *residue );
							}
						}
					}
				}

				for ( const std::unique_ptr<Residue> & residue : system.getResidues() )
				{
					for ( const size_t residueIndex : p_kwargs.residueIndexes )
					{
						if ( residue->getIndexInOriginalChain() == residueIndex )
						{
							if ( selectFullResidue )
								p_systemSelectionData.selectFullResidue( *residue );
							else
								p_systemSelectionData.referenceResidue( *residue );
						}
					}
				}
			}
			else
			{
				for ( const size_t chainID : chainIDs )
				{
					const Chain * chainPtr = system.getChain( chainID );
					const Chain & chain	   = *chainPtr;

					for ( const std::string & residueName : p_kwargs.residueNames )
					{
						const std::vector<Residue *> residues
							= App::Helper::Chemistry::findResiduesByName( chain, residueName );

						for ( Residue * const residue : residues )
						{
							if ( selectFullResidue )
								p_systemSelectionData.selectFullResidue( *residue );
							else
								p_systemSelectionData.referenceResidue( *residue );
						}
					}

					for ( const size_t residueIndex : p_kwargs.residueIndexes )
					{
						for ( const Residue & residue : chain.residues() )
						{
							if ( residue.getIndexInOriginalChain() == residueIndex )
							{
								if ( selectFullResidue )
									p_systemSelectionData.selectFullResidue( residue );
								else
									p_systemSelectionData.referenceResidue( residue );

								break;
							}
						}
					}
				}
			}
		}

		if ( !selectFullResidue )
		{
			_selectAtoms( p_kwargs, p_systemSelectionData );
		}
	}

	void SystemInterpretor::_selectAtoms(
		const InterpretedKwargs &	 p_kwargs,
		App::Selection::SystemData & p_systemSelectionData
	)
	{
		System & system = p_systemSelectionData.getSystem();

		const App::Selection::SystemData::IndexRangeList & residueIDs = p_systemSelectionData.getResidueIds();

		if ( residueIDs.isEmpty() )
		{
			const App::Selection::SystemData::IndexRangeList & chainIDs = p_systemSelectionData.getChainIds();

			if ( chainIDs.isEmpty() )
			{
				_addAtomsFollowingKwargs(
					0, atom_index_t( system.getAtoms().size() - 1 ), system, p_systemSelectionData, p_kwargs
				);
			}
			else
			{
				for ( const size_t chainID : chainIDs )
				{
					const Chain * const chain = system.getChain( chainID );

					if ( chain == nullptr )
						continue;

					_addAtomsFollowingKwargs(
						chain->getIndexFirstAtom(), chain->getIndexLastAtom(), system, p_systemSelectionData, p_kwargs
					);
				}
			}
		}
		else
		{
			const App::Selection::SystemData::IndexRangeList & residueIDs = p_systemSelectionData.getResidueIds();

			for ( const size_t residueID : residueIDs )
			{
				const Residue * const residue = system.getResidue( residueID );

				if ( residue == nullptr )
					continue;

				_addAtomsFollowingKwargs(
					residue->getIndexFirstAtom(), residue->getIndexLastAtom(), system, p_systemSelectionData, p_kwargs
				);
			}
		}
	}

	void SystemInterpretor::_addAtomsFollowingKwargs(
		const atom_index_t			 p_firstAtom,
		const atom_index_t			 p_lastAtom,
		System &					 p_system,
		App::Selection::SystemData & p_systemSelectionData,
		const InterpretedKwargs &	 p_kwargs
	)
	{
		for ( atom_index_t atomID = p_firstAtom; atomID <= p_lastAtom; atomID++ )
		{
			Atom * const atom = p_system.getAtom( atomID );

			if ( atom == nullptr )
				continue;

			for ( const std::string & atomName : p_kwargs.atomNames )
			{
				if ( atom->getName() == atomName )
				{
					p_systemSelectionData.selectAtom( *atom );
					break;
				}
			}

			for ( const VTX::Core::ChemDB::Atom::SYMBOL atomSymbol : p_kwargs.atomSymbols )
			{
				if ( atom->getSymbol() == atomSymbol )
				{
					p_systemSelectionData.selectAtom( *atom );
					break;
				}
			}
		}
	}

} // namespace VTX::App::PythonBinding::Selection
