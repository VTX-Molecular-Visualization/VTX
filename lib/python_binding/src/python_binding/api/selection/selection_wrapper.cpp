#include "python_binding/api/selection/selection_wrapper.hpp"
#ifdef JEVEUPAS

#include <app/application/scene.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/component/chemistry/system.hpp>
#include <app/component/scene/selectable.hpp>
#include <app/helper/chemistry.hpp>
#include <app/selection/selection_manager.hpp>
#include <app/selection/system_data.hpp>
#include <app/vtx_app.hpp>
#include <sstream>

namespace VTX::PythonBinding::API::Selection
{

	SelectionWrapper::SelectionWrapper() = default;
	SelectionWrapper::SelectionWrapper( const Selection & p_selection ) :
		_selection( std::make_unique<Selection>( p_selection ) )
	{
	}

	SelectionWrapper SelectionWrapper::intersect( const SelectionWrapper & p_lhs, const SelectionWrapper & p_rhs )
	{
		return SelectionWrapper( Selection::intersection( *( p_lhs._selection ), *( p_rhs._selection ) ) );
	}
	SelectionWrapper SelectionWrapper::exclusive( const SelectionWrapper & p_lhs, const SelectionWrapper & p_rhs )
	{
		return SelectionWrapper( Selection::exclusive( *( p_lhs._selection ), *( p_rhs._selection ) ) );
	}

	SelectionWrapper & SelectionWrapper::add( const SelectionWrapper & p_other )
	{
		Selection res = Selection::add( *_selection, *( p_other._selection ) );
		_selection	  = std::make_unique<Selection>( res );

		return *this;
	}
	SelectionWrapper & SelectionWrapper::remove( const SelectionWrapper & p_other )
	{
		Selection res = Selection::remove( *_selection, *( p_other._selection ) );
		_selection	  = std::make_unique<Selection>( res );

		return *this;
	}
	SelectionWrapper & SelectionWrapper::inverse()
	{
		SelectionWrapper all = SelectionWrapper();
		all.selectAll();

		//_selection = Selection::remove( all._selection, _selection );
		return *this;
	}

	SelectionWrapper & SelectionWrapper::save( const std::string & p_name )
	{
		App::SELECTION_MANAGER().save( p_name, std::make_unique<SelectionObj>( *_selection ) );
		return *this;
	}

	void SelectionWrapper::selectAll() {}

	const std::vector<App::Component::Chemistry::System *> SelectionWrapper::getSystems()
	{
		std::vector<App::Component::Chemistry::System *> res = std::vector<App::Component::Chemistry::System *>();

		for ( auto it = _selection->begin<App::Component::Chemistry::System>();
			  it != _selection->end<App::Component::Chemistry::System>();
			  it++ )
		{
			App::Component::Chemistry::System & mol
				= App::ECS_REGISTRY().getComponent<App::Component::Chemistry::System>( *it );
			res.emplace_back( &mol );
		}

		return res;
	}
	const std::vector<App::Component::Chemistry::Chain *> SelectionWrapper::getChains()
	{
		std::vector<App::Component::Chemistry::Chain *> res = std::vector<App::Component::Chemistry::Chain *>();

		for ( auto it = _selection->begin<App::Component::Chemistry::System>();
			  it != _selection->end<App::Component::Chemistry::System>();
			  it++ )
		{
			App::Component::Chemistry::System & mol = *it;

			const App::Selection::SystemData & molSelection
				//= PythonFixture::ECS_REGISTRY().getComponent<PythonFixture::Application::Selection::SystemData>( *it
				//);
				= _selection->getSelectionDataFromComponent<App::Selection::SystemData>( mol
				); // TODO : is this working properly ?

			for ( const size_t & chainID : molSelection.getChainIds() )
			{
				res.emplace_back( mol.getChain( chainID ) );
			}
		}

		return res;
	}
	const std::vector<App::Component::Chemistry::Residue *> SelectionWrapper::getResidues()
	{
		std::vector<App::Component::Chemistry::Residue *> res = std::vector<App::Component::Chemistry::Residue *>();

		for ( auto it = _selection->begin<App::Component::Chemistry::System>();
			  it != _selection->end<App::Component::Chemistry::System>();
			  it++ )
		{
			App::Component::Chemistry::System & mol = *it;

			const App::Selection::SystemData & molSelection
				//= PythonFixture::ECS_REGISTRY().getComponent<PythonFixture::Application::Selection::SystemData>( *it
				//);
				= _selection->getSelectionDataFromComponent<App::Selection::SystemData>( mol
				); // TODO : is this working properly ?

			for ( const size_t & residueID : molSelection.getResidueIds() )
			{
				res.emplace_back( mol.getResidue( residueID ) );
			}
		}

		return res;
	}
	const std::vector<App::Component::Chemistry::Atom *> SelectionWrapper::getAtoms()
	{
		std::vector<App::Component::Chemistry::Atom *> res = std::vector<App::Component::Chemistry::Atom *>();

		for ( auto it = _selection->begin<App::Component::Chemistry::System>();
			  it != _selection->end<App::Component::Chemistry::System>();
			  it++ )
		{
			App::Component::Chemistry::System & mol = *it;

			const App::Selection::SystemData & molSelection
				= _selection->getSelectionDataFromComponent<App::Selection::SystemData>( mol );

			for ( const atom_index_t & atomID : molSelection.getAtomIds() )
			{
				res.emplace_back( mol.getAtom( atomID ) );
			}
		}

		return res;
	}

	std::string SelectionWrapper::toString() const { return _selection->toString(); }

} // namespace VTX::PythonBinding::API::Selection
#endif // JEVEUPAS
