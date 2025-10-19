#ifndef __VTX_APP_SCENE_CAMERA__
#define __VTX_APP_SCENE_CAMERA__

#include <util/constants.hpp>

namespace VTX::App::Scene
{
	// Constants.
	constexpr Vec3f CAMERA_RIGHT_DEFAULT = RIGHT_AXIS;
	constexpr Vec3f CAMERA_UP_DEFAULT	 = UP_AXIS;
	constexpr Vec3f CAMERA_FRONT_DEFAULT = FRONT_AXIS;

	class Camera
	{
	  public:
		// Projection enum.
		enum class PROJECTION : int
		{
			PERSPECTIVE,
			ORTHOGRAPHIC,

			COUNT
		};

		Camera();

		inline const Mat4f & getViewMatrix() const { return _viewMatrix; }
		inline const Mat4f & getProjectionMatrix() const { return _projectionMatrix; }

		inline const size_t getScreenWidth() const { return _screenWidth; }
		inline const size_t getScreenHeight() const { return _screenHeight; }
		inline const float	getAspectRatio() const { return _aspectRatio; }

		inline const float getNear() const { return _near; }
		inline const float getFar() const { return _far; }
		inline const float getFov() const { return _fov; }

		inline const PROJECTION & getProjection() { return _projection; }
		void					  setCameraProjection( const PROJECTION & p_projection );

		const bool isPerspective() const { return _projection == PROJECTION::PERSPECTIVE; }

		void setScreenSize( const size_t p_width, const size_t p_height );

		void setNear( const float p_near );
		void setFar( const float p_far );
		void setFov( const float p_fov );

	  protected:
		size_t _screenWidth	 = 0;
		size_t _screenHeight = 0;
		float  _aspectRatio	 = 1.f;
		float  _near		 = 1e-1f;
		float  _far			 = 1e4f;
		float  _fov			 = 60.f;

		PROJECTION _projection = PROJECTION::PERSPECTIVE;

		Mat4f _viewMatrix		= MAT4F_ID;
		Mat4f _projectionMatrix = MAT4F_ID;

		/*
		virtual void _updateViewMatrix();

		void _updateProjectionMatrix();
		void _computePerspectiveProjectionMatrix();
		void _computeOrthographicProjectionMatrix();
		*/

	}; // namespace Camera
} // namespace VTX::App::Scene
#endif
