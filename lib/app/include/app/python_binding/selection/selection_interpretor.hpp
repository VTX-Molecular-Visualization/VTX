#ifndef __VTX_APP_PYTHON_BINDING_SELECTION_SELECTION_INTERPRETOR__
#define __VTX_APP_PYTHON_BINDING_SELECTION_SELECTION_INTERPRETOR__

#include "_fwd.hpp"
#include "app/selection/selection.hpp"
#include <python_binding/api/arguments.hpp>
#include <vector>

namespace VTX::App::PythonBinding::Selection
{

	using PythonKwargs = VTX::PythonBinding::API::PythonKwargs;
	using Selection	   = App::Selection::Selection;

	class SelectionInterpretor
	{
	  private:
		using InterpretArgFunc = std::function<void( Selection &, const PythonKwargs & )>;

	  public:
		static SelectionWrapper select( const PythonKwargs & kwargs );

		static void addInterpretor( const InterpretArgFunc & p_interpretor );
		static void clear();

	  private:
		inline static std::vector<InterpretArgFunc> _selectionInterpretors = std::vector<InterpretArgFunc>();
	};

} // namespace VTX::App::PythonBinding::Selection
#endif
