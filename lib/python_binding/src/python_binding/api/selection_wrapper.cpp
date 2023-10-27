#include "python_binding/api/selection_wrapper.hpp"
#include <app/application/scene.hpp>
#include <app/application/selection/molecule_data.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/component/scene/selectable.hpp>
#include <app/helper/chemistry.hpp>
#include <app/vtx_app.hpp>
#include <sstream>

namespace VTX::PythonBinding::API::Selection
{
	SelectionWrapper::SelectionWrapper() = default;
	SelectionWrapper::SelectionWrapper( const SelectionObj & p_selection ) :
		_selection( std::make_unique<SelectionObj>( p_selection ) )
	{
	}

	void SelectionWrapper::addInterpretor( const InterpretArgFunc & p_interpretor )
	{
		_selectionInterpretors.emplace_back( p_interpretor );
	}

	// Function that manage a param that can be a single str or a list of str and convert it into a vector of str
	SelectionWrapper SelectionWrapper::select( const pybind11::kwargs & kwargs )
	{
		SelectionWrapper res = SelectionWrapper();

		if ( pybind11::len( kwargs ) == 0 )
			return res;

		for ( const InterpretArgFunc & p_func : _selectionInterpretors )
			p_func( *res._selection, kwargs );

		return res;
	};

	SelectionWrapper & SelectionWrapper::add( const SelectionWrapper & p_other )
	{
		//_selection = SelectionObj::add( _selection, p_other._selection );
		return *this;
	}
	SelectionWrapper & SelectionWrapper::remove( const SelectionWrapper & p_other )
	{
		//_selection = SelectionObj::remove( _selection, p_other._selection );
		return *this;
	}
	SelectionWrapper & SelectionWrapper::intersect( const SelectionWrapper & p_other )
	{
		//_selection = SelectionObj::intersection( _selection, p_other._selection );
		return *this;
	}
	SelectionWrapper & SelectionWrapper::exclusive( const SelectionWrapper & p_other )
	{
		//_selection = SelectionObj::exclusive( _selection, p_other._selection );
		return *this;
	}
	SelectionWrapper & SelectionWrapper::inverse()
	{
		SelectionWrapper all = SelectionWrapper();
		all.selectAll();

		//_selection = SelectionObj::remove( all._selection, _selection );
		return *this;
	}

	void SelectionWrapper::selectAll() {}

	const std::vector<App::Component::Chemistry::Molecule *> SelectionWrapper::getMolecules()
	{
		std::vector<App::Component::Chemistry::Molecule *> res = std::vector<App::Component::Chemistry::Molecule *>();

		for ( auto it = _selection->begin<App::Component::Chemistry::Molecule>();
			  it != _selection->end<App::Component::Chemistry::Molecule>();
			  it++ )
		{
			App::Component::Chemistry::Molecule & mol
				= App::VTXApp::get().MAIN_REGISTRY().getComponent<App::Component::Chemistry::Molecule>( *it );
			res.emplace_back( &mol );
		}

		return res;
	}
	const std::vector<App::Component::Chemistry::Chain *> SelectionWrapper::getChains()
	{
		std::vector<App::Component::Chemistry::Chain *> res = std::vector<App::Component::Chemistry::Chain *>();

		for ( auto it = _selection->begin<App::Component::Chemistry::Molecule>();
			  it != _selection->end<App::Component::Chemistry::Molecule>();
			  it++ )
		{
			App::Component::Chemistry::Molecule & mol = *it;

			const App::Application::Selection::MoleculeData & molSelection
				= App::VTXApp::get().MAIN_REGISTRY().getComponent<App::Application::Selection::MoleculeData>( *it );

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

		for ( auto it = _selection->begin<App::Component::Chemistry::Molecule>();
			  it != _selection->end<App::Component::Chemistry::Molecule>();
			  it++ )
		{
			App::Component::Chemistry::Molecule & mol = *it;

			const App::Application::Selection::MoleculeData & molSelection
				= App::VTXApp::get().MAIN_REGISTRY().getComponent<App::Application::Selection::MoleculeData>( *it );

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

		for ( auto it = _selection->begin<App::Component::Chemistry::Molecule>();
			  it != _selection->end<App::Component::Chemistry::Molecule>();
			  it++ )
		{
			App::Component::Chemistry::Molecule & mol = *it;

			const App::Application::Selection::MoleculeData & molSelection
				= dynamic_cast<const App::Application::Selection::MoleculeData &>( _selection->getSelectionData( *it )
				);

			for ( const size_t & atomID : molSelection.getAtomIds() )
			{
				res.emplace_back( mol.getAtom( atomID ) );
			}
		}

		return res;
	}

	std::string SelectionWrapper::toString()
	{
		std::stringstream ss = std::stringstream();

		ss << "Selection :" << _selection->getCount() << std::endl;

		// ss << "Molecules (" << _molecules.size() << ") :";
		// for ( const App::Component::Chemistry::Molecule * const mol : _molecules )
		//{
		//	ss << mol->getName();
		// }

		// ss << std::endl << "Chains (" << _chains.size() << ") :";
		// for ( const App::Component::Chemistry::Chain * const chain : _chains )
		//{
		//	if ( chain != nullptr )
		//		ss << chain->getName();
		// }

		// ss << std::endl << "Residues (" << _residues.size() << ") :";
		// for ( const App::Component::Chemistry::Residue * const res : _residues )
		//{
		//	if ( res != nullptr )
		//		ss << res->getName();
		// }

		// ss << std::endl << "Atoms :(" << _atoms.size() << ") :";
		// for ( const App::Component::Chemistry::Atom * const atom : _atoms )
		//{
		//	if ( atom != nullptr )
		//		ss << atom->getName();
		// }

		return ss.str();
	}

} // namespace VTX::PythonBinding::API::Selection
