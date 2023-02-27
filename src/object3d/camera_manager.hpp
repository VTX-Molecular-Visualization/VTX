#ifndef __VTX_CAMERA_MANAGER__
#define __VTX_CAMERA_MANAGER__

namespace VTX
{
	namespace Object3D
	{
		class Camera;
		class PerspectiveCamera;
		class OrthographicCamera;

		class CameraManager
		{
		  public:
			CameraManager();
			~CameraManager();

			bool						isPerspective() const;
			inline Camera *				getCamera() const { return _currentCamera; }
			inline PerspectiveCamera *	getPerspectiveCamera() const { return _perspectiveCamera; }
			inline OrthographicCamera * getOrthographicCamera() const { return _orthographicCamera; }

			Camera * getCameraCopy();

			void setPerspectiveCamera( const bool p_perspective );
			void toggle();

			void setCamera( Camera * p_camera );
			void refresh();

		  private:
			Camera *			 _currentCamera;
			PerspectiveCamera *	 _perspectiveCamera;
			OrthographicCamera * _orthographicCamera;

		}; // namespace OrthographicCamera
	}	   // namespace Object3D
} // namespace VTX
#endif
