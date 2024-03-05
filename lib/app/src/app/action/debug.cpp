#include "app/action/debug.hpp"
#include "app/application/scene.hpp"
#include "app/component/render/camera.hpp"
#include "app/component/scene/transform_component.hpp"
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
		const Component::Render::Camera &	camera			= SCENE().getCamera();
		const Component::Scene::Transform & cameraTransform = camera.getTransform();

		VTX_INFO( "Position: {}", cameraTransform.getPosition() );
		VTX_INFO( "Rotation: {}", cameraTransform.getRotation() );
		VTX_INFO( "Front: {}", cameraTransform.getFront() );
		VTX_INFO( "Right: {}", cameraTransform.getRight() );
		VTX_INFO( "Up: {}", cameraTransform.getUp() );

		const std::string projectionStr = camera.isPerspective() ? "Perspective" : "Orthographic";
		VTX_INFO( "Projection: {}", projectionStr );
	}

} // namespace VTX::App::Action::Debug
