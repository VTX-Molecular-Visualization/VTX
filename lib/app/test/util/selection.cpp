#include "selection.hpp"
#include <app/application/scene.hpp>
#include <app/core/ecs/base_entity.hpp>
#include <app/core/ecs/registry.hpp>
#include <app/python_binding/interpretor.hpp>
#include <app/selection/selection_manager.hpp>
#include <app/vtx_app.hpp>
#include <sstream>
#include <util/logger.hpp>
#include <util/types.hpp>

namespace VTX::App::Test::Util
{
	bool Selection::checkSelection(
		const std::string &	 p_selName,
		const std::string &	 p_command,
		const SelectionObj & p_expectedResult
	)
	{
		std::stringstream command = std::stringstream();
		command << "(" << p_command << ").save('" << p_selName << "')";

		std::future<std::string> ret;
		INTERPRETOR().runCommand( command.str(), ret );
		ret.wait();

		const SelectionObj & result = App::SELECTION_MANAGER().getSaved( p_selName );

		return result.isEqualsTo( p_expectedResult );
	}

	Selection::SelectionObj Selection::createSelection( const SelectionData & p_sourceItem )
	{
		SelectionObj res = SelectionObj();
		res.select( p_sourceItem.getSelectionComponent(), p_sourceItem );

		return res;
	}
	Selection::SelectionObj Selection::createSelection( const std::unique_ptr<SelectionData> & p_itemPtr )
	{
		return createSelection( *p_itemPtr );
	}

	std::unique_ptr<Selection::SystemData> Selection::generateSystemData(
		const std::string &				  p_molName,
		const std::vector<size_t> &		  p_chains,
		const std::vector<size_t> &		  p_residues,
		const std::vector<atom_index_t> & p_atoms
	)
	{
		const App::Core::ECS::BaseEntity entity = SCENE().getItem( p_molName );

		const Component::Scene::Selectable & selectableComponent
			= ECS_REGISTRY().getComponent<Component::Scene::Selectable>( entity );

		std::unique_ptr<SystemData> res = std::make_unique<SystemData>( selectableComponent );

		const bool systemFullySelected = p_chains.size() == 0 && p_residues.size() == 0 && p_atoms.size() == 0;

		if ( !systemFullySelected )
		{
			res->clear();

			if ( not p_chains.empty() )
				res->selectFullChains( VTX::Util::Math::RangeList<size_t>::fromList( p_chains ) );
			if ( not p_residues.empty() )
				res->selectFullResidues( VTX::Util::Math::RangeList<size_t>::fromList( p_residues ) );
			if ( not p_atoms.empty() )
				res->selectAtoms( VTX::Util::Math::RangeList<atom_index_t>::fromList( p_atoms ) );
		}

		return std::move( res );
	}

} // namespace VTX::App::Test::Util
