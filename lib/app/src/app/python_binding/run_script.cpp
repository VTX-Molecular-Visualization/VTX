#include "app/python_binding/run_script.hpp"
#include "app/python_binding/interpretor.hpp"
#include <pybind11/pybind11.h>
#include <python_binding/interpretor.hpp>

namespace VTX::App::PythonBinding
{
	namespace
	{
		class RunScriptAction
		{
		  public:
			RunScriptAction( std::string p_path ) : _path( std::move( p_path ) ) {}

			void execute()
			{
				INTERPRETOR().subscribe( [ &path = _path ]( VTX::PythonBinding::Interpretor & p_interpretor )
										 { p_interpretor.runScript( path ); } );
			}

		  private:
			std::string _path;
		};
	} // namespace

	RunScript::RunScript( FilePath p_ ) : _path( std::move( p_ ) ) {}
	void RunScript::bind( VTX::PythonBinding::PyTXModule & p_vtxModule )
	{
		p_vtxModule.commands().bindAction<RunScriptAction, std::string>(
			"runScript", "Execute the script located at the path provided in argument."
		);
	}

} // namespace VTX::App::PythonBinding
