#ifndef __VTX_PYTHON_API_SELECTION_WRAPPER__
#define __VTX_PYTHON_API_SELECTION_WRAPPER__

#include "python_binding/api/selection/helper.hpp"
#include <app/application/selection/selection.hpp>
#include <app/component/chemistry/_fwd.hpp>
#include <pybind11/pybind11.h>
#include <string>
#include <vector>

namespace VTX::PythonBinding::API::Selection
{
	class SelectionWrapper
	{
	  private:
		using InterpretArgFunc = std::function<void( SelectionObj &, const pybind11::kwargs & )>;

	  public:
		static SelectionWrapper select( const pybind11::kwargs & kwargs );

		static void addInterpretor( const InterpretArgFunc & p_interpretor );

	  public:
		SelectionWrapper();
		SelectionWrapper( const SelectionObj & p_selection );

		SelectionWrapper & add( const SelectionWrapper & p_other );
		SelectionWrapper & remove( const SelectionWrapper & p_other );
		SelectionWrapper & intersect( const SelectionWrapper & p_other );
		SelectionWrapper & exclusive( const SelectionWrapper & p_other );
		SelectionWrapper & inverse();

		void selectAll();

		const std::vector<App::Component::Chemistry::Molecule *> getMolecules();
		const std::vector<App::Component::Chemistry::Chain *>	 getChains();
		const std::vector<App::Component::Chemistry::Residue *>	 getResidues();
		const std::vector<App::Component::Chemistry::Atom *>	 getAtoms();

		std::string toString();

	  private:
		std::unique_ptr<App::Application::Selection::Selection> _selection
			= std::make_unique<App::Application::Selection::Selection>();

		inline static std::vector<InterpretArgFunc> _selectionInterpretors = std::vector<InterpretArgFunc>();
	};

} // namespace VTX::PythonBinding::API::Selection
#endif
