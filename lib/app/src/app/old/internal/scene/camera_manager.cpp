#include "app/old/internal/scene/camera_manager.hpp"
#include "app/old/component/render/camera.hpp"
#include "app/old/component/render/enum_camera.hpp"
#include "app/old/vtx_app.hpp"

namespace VTX::App::Old::Internal::Scene
{
	CameraManager::CameraManager()
	{
		_currentCamera = new App::Old::Component::Render::Camera();
		_currentCamera->setScreenSize( CAMERA_WIDTH_DEFAULT, CAMERA_HEIGHT_DEFAULT );
	}
	CameraManager::~CameraManager() { delete _currentCamera; }

	void CameraManager::setPerspectiveCamera( const bool p_perspective )
	{
		const App::Old::Component::Render::CAMERA_PROJECTION projection
			= p_perspective ? App::Old::Component::Render::CAMERA_PROJECTION::PERSPECTIVE
							: App::Old::Component::Render::CAMERA_PROJECTION::ORTHOGRAPHIC;

		_currentCamera->setCameraProjection( projection );
	}
	void CameraManager::toggle()
	{
		const App::Old::Component::Render::CAMERA_PROJECTION currentProjection = _currentCamera->getProjection();
		const App::Old::Component::Render::CAMERA_PROJECTION nextProjection	  = App::Old::Component::Render::CAMERA_PROJECTION(
			   ( int( currentProjection ) + 1 ) % int( App::Old::Component::Render::CAMERA_PROJECTION::COUNT ) );

		_currentCamera->setCameraProjection( nextProjection );
	}

	void CameraManager::refresh()
	{
		if ( VTX_SETTING().getCameraPerspective() == _currentCamera->isPerspective() )
			return;

		const bool										cameraPerspective = VTX_SETTING().getCameraPerspective();
		const App::Old::Component::Render::CAMERA_PROJECTION projection
			= cameraPerspective ? App::Old::Component::Render::CAMERA_PROJECTION::PERSPECTIVE
								: App::Old::Component::Render::CAMERA_PROJECTION::ORTHOGRAPHIC;

		_currentCamera->setCameraProjection( projection );
	}
} // namespace VTX::App::Old::Internal::Scene
