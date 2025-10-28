#ifndef __VTX_APP_SCENE_CAMERA__
#define __VTX_APP_SCENE_CAMERA__

#include "app/services.hpp"
#include "app/settings/settings.hpp"
#include <util/constants.hpp>

namespace VTX::App::Scene
{
	/**
	 * @brief Defines a camera.
	 */
	class Camera
	{
	  public:
		/**
		 * @brief Projection types.
		 */
		enum class PROJECTION : int
		{
			PERSPECTIVE,
			ORTHOGRAPHIC,

			COUNT
		};

		/**
		 * @brief Screen size.
		 */
		size_t screenWidth	= 1;
		size_t screenHeight = 1;

		/**
		 * @brief Camera parameters.
		 */
		float	   near		  = Settings::Camera::NEAR_DEFAULT;
		float	   far		  = Settings::Camera::FAR_DEFAULT;
		float	   fov		  = Settings::Camera::FOV_DEFAULT;
		PROJECTION projection = PROJECTION( Settings::Camera::PROJECTION_DEFAULT );

		/**
		 * @brief Compute projection matrix.
		 */
		Mat4f computeProjectionMatrix() const;
	}; // namespace Camera
} // namespace VTX::App::Scene
#endif
