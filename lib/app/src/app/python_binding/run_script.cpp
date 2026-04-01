#include "app/python_binding/run_script.hpp"
#include "app/python_binding/interpretor.hpp"
#include "app/services.hpp"
#include <pybind11/pybind11.h>
#include <python_binding/interpretor.hpp>
#include <util/logger.hpp>

namespace VTX::App::PythonBinding
{
	namespace
	{
		class RunScriptAction
		{
		  public:
			RunScriptAction() {}

			void execute( std::string p_path )
			{
				std::shared_ptr<std::promise<Interpretor::AsyncJobResult>> promise
					= std::make_shared<std::promise<Interpretor::AsyncJobResult>>();
				std::future<Interpretor::AsyncJobResult> _future = promise->get_future();
				INTERPRETOR().runScript( p_path, promise );
				if ( _future.get().success == false )
					throw pybind11::value_error( _future.get().resultStr );
			}
		};
	} // namespace

	void RunScript::bind( VTX::PythonBinding::Wrapper::Module & p_vtxModule )
	{
		p_vtxModule.bindAction<RunScriptAction, std::string>(
			"runScript", "Execute the script located at the path provided in argument."
		);
	}

} // namespace VTX::App::PythonBinding
