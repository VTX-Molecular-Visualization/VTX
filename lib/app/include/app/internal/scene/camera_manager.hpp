#ifndef __VTX_APP_INTERNAL_SCENE_CAMERA_MANAGER__
#define __VTX_APP_INTERNAL_SCENE_CAMERA_MANAGER__

#include "app/component/render/_fwd.hpp"

namespace VTX::App::Internal::Scene
{
	class CameraManager
	{
	  private:
		inline static const int CAMERA_WIDTH_DEFAULT  = 1280;
		inline static const int CAMERA_HEIGHT_DEFAULT = 720;

	  public:
		CameraManager();
		~CameraManager();

		inline App::Component::Render::Camera * getCamera() const { return _currentCamera; }

		void setPerspectiveCamera( const bool p_perspective );
		void toggle();

		void refresh();

	  private:
		App::Component::Render::Camera * _currentCamera;

	}; // namespace OrthographicCamera
} // namespace VTX::App::Internal::Scene
#endif
