#include "app/python_binding/app_binder.hpp"
#include "app/python_binding/application.hpp"
#include "app/python_binding/camera.hpp"
#include "app/python_binding/color.hpp"
#include "app/python_binding/io.hpp"
#include "app/python_binding/representation.hpp"
#include "app/python_binding/scene.hpp"
#include "app/python_binding/selection.hpp"
#include "app/python_binding/system.hpp"
#include "app/python_binding/topology/binder.hpp"
#include "app/python_binding/visibility.hpp"
#include <util/logger.hpp>

namespace VTX::App::PythonBinding
{
	void AppBinder::bind( VTX::PythonBinding::Wrapper::Module & p_vtxmodule )
	{
		VTX::VTX_DEBUG( "Applying binding on module." );

		Module appModule( p_vtxmodule );

		SceneBinder().bind( appModule );
		IOBinder().bind( appModule );
		SystemBinder().bind( appModule );
		TopologyBinder().bind( appModule );
		CameraBinder().bind( appModule );
		SelectionBinder().bind( appModule );
		VisibilityBinder().bind( appModule );
		ColorBinder().bind( appModule );
		RepresentationBinder().bind( appModule );
		ApplicationBinder().bind( appModule );
	}
} // namespace VTX::App::PythonBinding
