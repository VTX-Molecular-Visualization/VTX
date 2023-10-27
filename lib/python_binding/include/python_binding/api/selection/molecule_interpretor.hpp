#ifndef __VTX_PYTHON_API_SELECTION_MOLECULE_INTERPRETOR__
#define __VTX_PYTHON_API_SELECTION_MOLECULE_INTERPRETOR__

#include <app/application/selection/selection.hpp>
#include <app/component/chemistry/_fwd.hpp>
#include <pybind11/pybind11.h>
#include <set>

namespace VTX::PythonBinding::API::Selection
{
	class MoleculeInterpretor
	{
	  public:
		static void interpretMolecules( App::Application::Selection::Selection &, const pybind11::kwargs & );

	  private:
		static std::set<App::Component::Chemistry::Molecule *> _getMolecules( const pybind11::kwargs & p_kwargs );
		static bool _hasMoleculeOptionsInKwargs( const pybind11::kwargs & p_kwargs );
	};

} // namespace VTX::PythonBinding::API::Selection
#endif
