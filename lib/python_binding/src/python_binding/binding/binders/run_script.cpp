#include "python_binding/binding/binders/run_script.hpp"
#include <pybind11/pybind11.h>

namespace VTX::PythonBinding::Binding::Binders
{
	RunScript::RunScript( FilePath p_ ) : _path( std::move( p_ ) ) {}
	void RunScript::bind( PythonBinding::PyTXModule & p_vtxModule )
	{
		p_vtxModule.commands(); // TODO
	}

} // namespace VTX::PythonBinding::Binding::Binders
