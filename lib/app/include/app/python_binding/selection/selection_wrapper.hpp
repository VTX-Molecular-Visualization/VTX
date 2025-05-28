#ifndef __VTX_APP_PYTHON_BINDING_SELECTION_SELECTION_WRAPPER__
#define __VTX_APP_PYTHON_BINDING_SELECTION_SELECTION_WRAPPER__

#include "_fwd.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/component/chemistry/system.hpp"
#include "app/python_binding/selection/helper.hpp"
#include "app/selection/selection.hpp"
#include <string>
#include <vector>

namespace VTX::App::PythonBinding::Selection
{

	using Selection = App::Selection::Selection;

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

} // namespace VTX::App::PythonBinding::Selection
#endif
