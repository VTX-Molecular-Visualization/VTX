#include "app/old_app/object3d/camera_manager.hpp"
#include "app/old_app/object3d/camera.hpp"
#include "app/old_app/vtx_app.hpp"

namespace VTX
{
	namespace Object3D
	{
		CameraManager::CameraManager() { _currentCamera = new Camera(); }
		CameraManager::~CameraManager() { delete _currentCamera; }

		void CameraManager::setPerspectiveCamera( const bool p_perspective )
		{
			const CameraProjection projection
				= p_perspective ? CameraProjection::PERSPECTIVE : CameraProjection::ORTHOGRAPHIC;

			_currentCamera->setCameraProjection( projection );
		}
		void CameraManager::toggle()
		{
			const CameraProjection currentProjection = _currentCamera->getProjection();
			const CameraProjection nextProjection
				= CameraProjection( ( int( currentProjection ) + 1 ) % int( CameraProjection::COUNT ) );

			_currentCamera->setCameraProjection( nextProjection );
		}

		void CameraManager::refresh()
		{
			if ( VTX_SETTING().getCameraPerspective() == _currentCamera->isPerspective() )
				return;

			const bool			   cameraPerspective = VTX_SETTING().getCameraPerspective();
			const CameraProjection projection
				= cameraPerspective ? CameraProjection::PERSPECTIVE : CameraProjection::ORTHOGRAPHIC;

			_currentCamera->setCameraProjection( projection );
		}
	} // namespace Object3D
} // namespace VTX
