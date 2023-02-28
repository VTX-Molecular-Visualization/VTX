#ifndef __VTX_CAMERA_MANAGER__
#define __VTX_CAMERA_MANAGER__

namespace VTX
{
	namespace Object3D
	{
		class Camera;

		class CameraManager
		{
		  public:
			CameraManager();
			~CameraManager();

			inline Camera * getCamera() const { return _currentCamera; }

			void setPerspectiveCamera( const bool p_perspective );
			void toggle();

			void refresh();

		  private:
			Camera * _currentCamera;

		}; // namespace OrthographicCamera
	}	   // namespace Object3D
} // namespace VTX
#endif
