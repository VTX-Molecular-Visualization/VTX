#include "representation_manager.hpp"
#include "event/event_manager.hpp"
#include "generic/base_representable.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/representation/instantiated_representation.hpp"
#include "model/representation/representation.hpp"
#include "model/representation/representation_library.hpp"
#include "model/residue.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "setting.hpp"

namespace VTX::Representation
{
	RepresentationManager::RepresentationManager()
	{
		setDefaultRepresentationIndex( VTX::Setting::REPRESENTATION_DEFAULT_INDEX );
	}

	RepresentationManager::InstantiatedRepresentation * RepresentationManager::instantiateDefaultRepresentation()
	{
		return MVC::MvcManager::get().instantiateModel<InstantiatedRepresentation>( _defaultBaseRepresentation );
	}
	void RepresentationManager::setDefaultRepresentationIndex( const int p_defaultRepresentationIndex )
	{
		_defaultBaseRepresentation
			= Model::Representation::RepresentationLibrary::get().getRepresentation( p_defaultRepresentationIndex );

		InstantiatedRepresentation * instantiatedDefautRep
			= MVC::MvcManager::get().instantiateModel<InstantiatedRepresentation>( _defaultBaseRepresentation );
		_defaultRepresentationSet.clear();
		_defaultRepresentationSet.emplace( instantiatedDefautRep );
	}

	void RepresentationManager::instantiateRepresentation( BaseRepresentation * const	p_representation,
														   Generic::BaseRepresentable & p_target )
	{
		Model::Representation::InstantiatedRepresentation * const instantiatedRepresentation
			= MVC::MvcManager::get().instantiateModel<Model::Representation::InstantiatedRepresentation>(
				p_representation );

		p_target.applyRepresentation( instantiatedRepresentation );
	}

	void RepresentationManager::instantiateRepresentation( BaseRepresentation * const p_representation,
														   const Model::Selection &	  p_selection )
	{
		for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & molData : p_selection.getItems() )
		{
			Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( molData.first );
			if ( molData.second.getFullySelectedChildCount() == molecule.getChainCount() )
			{
				InstantiatedRepresentation * const instantiatedRepresentation
					= MVC::MvcManager::get().instantiateModel<Model::Representation::InstantiatedRepresentation>(
						p_representation );

				molecule.applyRepresentation( instantiatedRepresentation );
				continue;
			}

			for ( const std::pair<Model::ID, Model::Selection::MapResidueIds> & chainData : molData.second )
			{
				Model::Chain & chain = *molecule.getChain( chainData.first );
				if ( chainData.second.getFullySelectedChildCount() == chain.getResidueCount() )
				{
					InstantiatedRepresentation * const instantiatedRepresentation
						= MVC::MvcManager::get().instantiateModel<Model::Representation::InstantiatedRepresentation>(
							p_representation );

					chain.applyRepresentation( instantiatedRepresentation );
					continue;
				}

				for ( const std::pair<Model::ID, Model::Selection::VecAtomIds> & residueData : chainData.second )
				{
					Model::Residue & residue = *molecule.getResidue( residueData.first );

					InstantiatedRepresentation * const instantiatedRepresentation
						= MVC::MvcManager::get().instantiateModel<Model::Representation::InstantiatedRepresentation>(
							p_representation );

					residue.applyRepresentation( instantiatedRepresentation );
				}
			}
		}
	}

	void RepresentationManager::deleteRepresentation( const BaseRepresentation *& p_representation )
	{
		delete p_representation;
		p_representation = nullptr;
	}

	Model::Representation::InstantiatedRepresentation * const RepresentationManager::getRepresentationByName(
		const std::string & p_representationName )
	{
		InstantiatedRepresentation * res = nullptr;

		for ( std::pair<InstantiatedRepresentation *, std::unordered_set<Generic::BaseRepresentable *>> item :
			  _mapRepresentablesLinkedToRepresentation )
		{
			if ( item.first->getName().compare( p_representationName ) == 0 )
			{
				res = item.first;
				break;
			}
		}

		return res;
	}

} // namespace VTX::Representation
