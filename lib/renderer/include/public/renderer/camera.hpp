#ifndef __VTX_RENDERER_CAMERA__
#define __VTX_RENDERER_CAMERA__

#include <util/math.hpp>

namespace VTX::Renderer
{
	/**
	 * @brief Projection types.
	 */
	enum struct PROJECTION
	{
		PERSPECTIVE,
		ORTHOGRAPHIC,

		COUNT

	};

	enum struct E_CAMERA_VALUES
	{
		NEAR_CLIP,
		FAR_CLIP,
		FOV,
		PROJECTION
	};

	/**
	 * @brief Default values.
	 */
	constexpr float NEAR_CLIP_DEFAULT = 1e-1f;
	constexpr float NEAR_CLIP_MIN	  = 1e-1f;
	constexpr float NEAR_CLIP_MAX	  = 1e4f;

	constexpr float FAR_CLIP_DEFAULT = 1e4f;
	constexpr float FAR_CLIP_MIN	 = 1e-1f;
	constexpr float FAR_CLIP_MAX	 = 1e4f;

	constexpr float FOV_DEFAULT = 45.f;
	constexpr float FOV_MIN		= 10.f;
	constexpr float FOV_MAX		= 90.f;

	constexpr PROJECTION PROJECTION_DEFAULT = PROJECTION::PERSPECTIVE;

	/**
	 * @brief Defines a camera.
	 */
	struct Camera
	{
	  public:
		/**
		 * @brief Camera settings.
		 */
		float	   near;
		float	   far;
		float	   fov;
		PROJECTION projection;

		/**
		 * @brief Screen size.
		 */
		size_t screenWidth;
		size_t screenHeight;

		/**
		 * @brief Compute projection matrix.
		 */
		inline Mat4f computeProjectionMatrix() const
		{
			using namespace Util;

			assert( screenWidth > 0.f && screenHeight > 0.f );

			switch ( projection )
			{
			case PROJECTION::PERSPECTIVE:
			{
				return Math::perspective(
					Math::radians( fov ), float( screenWidth ) / float( screenHeight ), near, far
				);
			}

			case PROJECTION::ORTHOGRAPHIC:
			{
				const float halfHeight = screenHeight * 0.5f;
				const float halfWidth  = screenWidth * 0.5f;
				return Math::ortho( -halfWidth, halfWidth, -halfHeight, halfHeight, near, far );
			}

			default:
			{
				assert( false );
				return MAT4F_ID;
			}
			}
		}

	}; // namespace Camera
} // namespace VTX::Renderer
#endif
