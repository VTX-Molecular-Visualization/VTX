#include "app/python_binding/application.hpp"
#include "app/action/application.hpp"
#include "app/services.hpp"
#include <cstddef>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <python_binding/wrapper/arg.hpp>
#include <renderer/renderer.hpp>
#include <utility>

namespace VTX::App::PythonBinding
{
	namespace
	{
		void _setResolution( const size_t p_width, const size_t p_height )
		{
			if ( p_width == 0 || p_height == 0 )
			{
				throw pybind11::value_error( "Resolution must be greater than zero." );
			}
			executeAction<App::Action::Application::Resize>( p_width, p_height );
		}

		std::pair<size_t, size_t> _getResolution() { return { RENDERER().width(), RENDERER().height() }; }
	} // namespace

	void ApplicationBinder::bind( Module & p_vtxModule )
	{
		p_vtxModule.bindAction<App::Action::Application::Quit>( "quit", "Quit the application." );
		p_vtxModule.def(
			"setResolution",
			&_setResolution,
			"Set the renderer resolution in pixels.",
			VTX::PythonBinding::Wrapper::Arg( "width" ),
			VTX::PythonBinding::Wrapper::Arg( "height" )
		);
		p_vtxModule.def(
			"getResolution",
			synchronized( &_getResolution ),
			"Get the renderer resolution in pixels as (width, height)."
		);
	}
} // namespace VTX::App::PythonBinding
