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
			RunScriptAction( std::string p_path ) : _path( std::move( p_path ) ) {}

			void execute()
			{
				std::shared_ptr<std::promise<Interpretor::AsyncJobResult>> promise
					= std::make_shared<std::promise<Interpretor::AsyncJobResult>>();
				std::future<Interpretor::AsyncJobResult> _future = promise->get_future();
				INTERPRETOR().runScript( _path, promise );
				if ( _future.get().success == false )
					throw pybind11::value_error( _future.get().resultStr );
			}

		  private:
			std::string _path;
		};
	} // namespace

	void RunScript::bind( VTX::PythonBinding::PyTXModule & p_vtxModule )
	{
		p_vtxModule.commands().bindAction<RunScriptAction, std::string>(
			"runScript", "Execute the script located at the path provided in argument."
		);
	}

} // namespace VTX::App::PythonBinding
