#ifndef __VTX_APP_PYTHON_BINDING_PRESET_TYPES__
#define __VTX_APP_PYTHON_BINDING_PRESET_TYPES__

#include "app/ecs.hpp"

namespace VTX::Renderer
{
	struct GraphicsConfig;
	struct Representation;

	namespace Color
	{
		struct Layout;
	}
}

namespace VTX::App::PythonBinding::Preset
{
	template<typename T>
	struct Handle
	{
		Entity entity = InvalidEntity;
	};

	using ColorLayoutHandle	   = Handle<Renderer::Color::Layout>;
	using GraphicsConfigHandle = Handle<Renderer::GraphicsConfig>;
	using RepresentationHandle = Handle<Renderer::Representation>;
}

#endif
