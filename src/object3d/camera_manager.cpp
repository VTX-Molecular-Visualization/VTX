#include "camera_manager.hpp"
#include "camera.hpp"
#include "orthographic_camera.hpp"
#include "perspective_camera.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Object3D
	{
		CameraManager::CameraManager()
		{
			_perspectiveCamera	= new PerspectiveCamera();
			_orthographicCamera = new OrthographicCamera();

			if ( VTX_SETTING().getCameraPerspective() )
				_currentCamera = _perspectiveCamera;
			else
				_currentCamera = _orthographicCamera;
		}

		CameraManager::~CameraManager()
		{
			delete _perspectiveCamera;
			delete _orthographicCamera;
		}

		bool CameraManager::isPerspective() const { return _currentCamera->isPerspective(); }

		Camera * CameraManager::getCameraCopy()
		{
			if ( _currentCamera->isPerspective() )
			{
				PerspectiveCamera * pCopy = new PerspectiveCamera();
				*pCopy					  = *_perspectiveCamera;
				return pCopy;
			}
			else
			{
				OrthographicCamera * oCopy = new OrthographicCamera();
				*oCopy					   = *_orthographicCamera;
				return oCopy;
			}
		}

		void CameraManager::setPerspectiveCamera( const bool p_perspective )
		{
			if ( p_perspective )
			{
				_currentCamera = _perspectiveCamera;
			}
			else
			{
				_currentCamera = _orthographicCamera;
			}
		}
		void CameraManager::toggle() { setPerspectiveCamera( !isPerspective() ); }

		void CameraManager::setCamera( Camera * p_camera )
		{
			if ( p_camera->isPerspective() )
			{
				_perspectiveCamera = static_cast<PerspectiveCamera *>( p_camera );
				_currentCamera	   = _perspectiveCamera;
			}
			else
			{
				_orthographicCamera = static_cast<OrthographicCamera *>( p_camera );
				_currentCamera		= _orthographicCamera;
			}
		}

		void CameraManager::refresh()
		{
			if ( VTX_SETTING().getCameraPerspective() == _currentCamera->isPerspective() )
				return;

			if ( VTX_SETTING().getCameraPerspective() )
			{
				_perspectiveCamera->fromOrthographic( _orthographicCamera );
				_currentCamera = _perspectiveCamera;
			}
			else
			{
				_orthographicCamera->fromPerspective( _perspectiveCamera );
				_currentCamera = _orthographicCamera;
			}
		}
	} // namespace Object3D
} // namespace VTX
