#ifndef __VTX_APP_SCENE_CAMERA__
#define __VTX_APP_SCENE_CAMERA__

#include <util/types.hpp>

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
		enum struct PROJECTION
		{
			PERSPECTIVE,
			ORTHOGRAPHIC,

			COUNT
		};

		/**
		 * @brief Camera settings.
		 */
		const float *	   near;
		const float *	   far;
		const float *	   fov;
		const PROJECTION * projection;

		/**
		 * @brief Screen size.
		 */
		size_t screenWidth;
		size_t screenHeight;

		/**
		 * @brief Constructor.
		 */
		Camera();

		/**
		 * @brief Compute projection matrix.
		 */
		Mat4f computeProjectionMatrix() const;

	}; // namespace Camera
} // namespace VTX::App::Scene
#endif
