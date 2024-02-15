#include "app/action/debug.hpp"
#include "app/application/scene.hpp"
#include "app/component/render/camera.hpp"
#include "app/vtx_app.hpp"
#include <string>
#include <util/exceptions.hpp>
#include <util/fmt/glm.hpp>
#include <util/logger.hpp>

namespace VTX::App::Action::Debug
{
	void ToggleRendererActivation::execute() { throw NotImplementedException(); }
	void RefreshSES::execute() { throw NotImplementedException(); }
	void ChangeRenderMode::execute() { throw NotImplementedException(); }

	void PrintCameraInfo::execute()
	{
		const Component::Render::Camera & camera = SCENE().getCamera();

		VTX_INFO( "Position: {}", camera.getPosition() );
		VTX_INFO( "Rotation: {}", camera.getRotation() );
		VTX_INFO( "Front: {}", camera.getFront() );
		VTX_INFO( "Right: {}", camera.getRight() );
		VTX_INFO( "Up: {}", camera.getUp() );

		const std::string projectionStr = camera.isPerspective() ? "Perspective" : "Orthographic";
		VTX_INFO( "Projection: {}", projectionStr );
	}

} // namespace VTX::App::Action::Debug
