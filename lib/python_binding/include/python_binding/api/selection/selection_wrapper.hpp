#ifndef __VTX_PYTHON_API_SELECTION_SELECTION_WRAPPER__
#define __VTX_PYTHON_API_SELECTION_SELECTION_WRAPPER__

#include "_fwd.hpp"
#include "python_binding/api/selection/helper.hpp"
#include <app/application/selection/selection.hpp>
#include <app/component/chemistry/_fwd.hpp>
#include <pybind11/pybind11.h>
#include <string>
#include <vector>

namespace VTX::PythonBinding::API::Selection
{
	using Selection = App::Application::Selection::Selection;

	class SelectionWrapper
	{
		friend SelectionInterpretor;

	  public:
		SelectionWrapper();
		SelectionWrapper( const Selection & p_selection );

		static SelectionWrapper intersect( const SelectionWrapper & p_lhs, const SelectionWrapper & p_rhs );
		static SelectionWrapper exclusive( const SelectionWrapper & p_lhs, const SelectionWrapper & p_rhs );

		SelectionWrapper & add( const SelectionWrapper & p_other );
		SelectionWrapper & remove( const SelectionWrapper & p_other );
		// SelectionWrapper & intersect( const SelectionWrapper & p_other );
		// SelectionWrapper & exclusive( const SelectionWrapper & p_other );
		SelectionWrapper & inverse();

		SelectionWrapper & save( const std::string & p_name );

		void selectAll();

		const std::vector<App::Component::Chemistry::System *>	getSystems();
		const std::vector<App::Component::Chemistry::Chain *>	getChains();
		const std::vector<App::Component::Chemistry::Residue *> getResidues();
		const std::vector<App::Component::Chemistry::Atom *>	getAtoms();

		std::string toString() const;

	  private:
		std::unique_ptr<Selection> _selection = std::make_unique<Selection>();
	};

} // namespace VTX::PythonBinding::API::Selection
#endif
