#include "selection.hpp"
#include <app/application/scene.hpp>
#include <app/application/selection/selection_manager.hpp>
#include <app/core/ecs/base_entity.hpp>
#include <app/core/ecs/registry.hpp>
#include <app/vtx_app.hpp>
#include <python_binding/interpretor.hpp>
#include <sstream>
#include <util/logger.hpp>

namespace VTX::App::Test::Util
{
	bool Selection::checkSelection(
		const std::string &	 p_selName,
		const std::string &	 p_command,
		const SelectionObj & p_expectedResult
	)
	{
		const VTX::PythonBinding::Interpretor & p_interpretor
			= VTXApp::get().getSystem().getSystem<VTX::PythonBinding::Interpretor>(
				VTX::PythonBinding::Interpretor::SYSTEM_KEY
			);

		std::stringstream command = std::stringstream();
		command << "(" << p_command << ").save('" << p_selName << "')";

		p_interpretor.runCommand( command.str() );

		const SelectionObj & result = VTXApp::get().getSelectionManager().getSaved( p_selName );

		return result.isEqualsTo( p_expectedResult );
	}

	Selection::SelectionObj Selection::createSelection( const SelectionData & p_sourceItem )
	{
		SelectionObj res = SelectionObj();

		SelectionData & selectionData = res.select( p_sourceItem.getSelectionComponent() );
		selectionData.add( p_sourceItem );

		return res;
	}
	Selection::SelectionObj Selection::createSelection( const std::unique_ptr<SelectionData> & p_itemPtr )
	{
		return createSelection( *p_itemPtr );
	}

	Selection::SelectionObj Selection::createSelection( const std::vector<SelectionData *> & p_items )
	{
		SelectionObj res = SelectionObj();

		for ( const SelectionData * const sourceItemPtr : p_items )
		{
			SelectionData & selectionData = res.select( sourceItemPtr->getSelectionComponent() );
			selectionData.add( *sourceItemPtr );
		}

		return res;
	}

	std::unique_ptr<Selection::MoleculeData> Selection::generateMoleculeData(
		const std::string &			p_molName,
		const std::vector<size_t> & p_chains,
		const std::vector<size_t> & p_residues,
		const std::vector<size_t> & p_atoms
	)
	{
		const App::Core::ECS::BaseEntity entity = VTXApp::get().getScene().getItem( p_molName );

		const Component::Scene::Selectable & selectableComponent
			= VTXApp::get().MAIN_REGISTRY().getComponent<Component::Scene::Selectable>( entity );

		std::unique_ptr<MoleculeData> res = std::make_unique<MoleculeData>( selectableComponent );

		if ( p_chains.size() == 0 && p_residues.size() == 0 && p_atoms.size() == 0 )
		{
			res->selectAll();
		}
		else
		{
			res->selectFullChains( VTX::Util::Math::RangeList<size_t>::fromList( p_chains ) );
			res->selectFullResidues( VTX::Util::Math::RangeList<size_t>::fromList( p_residues ) );
			res->selectAtoms( VTX::Util::Math::RangeList<size_t>::fromList( p_atoms ) );
		}

		return std::move( res );
	}

} // namespace VTX::App::Test::Util
