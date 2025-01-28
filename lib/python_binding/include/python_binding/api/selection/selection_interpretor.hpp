#ifndef __VTX_PYTHON_API_SELECTION_SELECTION_INTERPRETOR__
#define __VTX_PYTHON_API_SELECTION_SELECTION_INTERPRETOR__

#include "_fwd.hpp"
#include <app/component/chemistry/_fwd.hpp>
#include <app/selection/selection.hpp>
#include <pybind11/pybind11.h>

namespace VTX::PythonBinding::API::Selection
{
	using Selection = App::Selection::Selection;

	class SelectionInterpretor
	{
	  private:
		using InterpretArgFunc = std::function<void( Selection &, const pybind11::kwargs & )>;

	  public:
		static SelectionWrapper select( const pybind11::kwargs & kwargs );

		static void addInterpretor( const InterpretArgFunc & p_interpretor );
		static void clear();

	  private:
		inline static std::vector<InterpretArgFunc> _selectionInterpretors = std::vector<InterpretArgFunc>();
	};

} // namespace VTX::PythonBinding::API::Selection
#endif
