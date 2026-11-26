#ifndef VTX_APP_PYTHON_BINDING_PRESET_TYPES_HPP
#define VTX_APP_PYTHON_BINDING_PRESET_TYPES_HPP

#include "app/ecs.hpp"

namespace VTX::Renderer
{
	struct GraphicsConfig;
	struct Representation;

	namespace Color
	{
		struct Layout;
	}
} // namespace VTX::Renderer

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
} // namespace VTX::App::PythonBinding::Preset

#endif
