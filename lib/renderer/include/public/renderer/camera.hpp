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

	/*
	enum struct E_CAMERA_VALUES
	{
		NEAR_CLIP,
		FAR_CLIP,
		FOV,
		PROJECTION
	};
	*/

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
		/**
		 * @brief Camera settings.
		 */
		float	   near		  = NEAR_CLIP_DEFAULT;
		float	   far		  = FAR_CLIP_DEFAULT;
		float	   fov		  = FOV_DEFAULT;
		PROJECTION projection = PROJECTION_DEFAULT;

		/**
		 * @brief Screen size.
		 */
		size_t screenWidth;
		size_t screenHeight;

		/**
		 * @brief Target.
		 */
		Vec3f target;

		/**
		 * @brief Compute projection matrix.
		 */
		inline Mat4f computeProjectionMatrix( const Vec3f & p_position ) const
		{
			using namespace Util;

			assert( screenWidth > 0.f && screenHeight > 0.f );

			float aspectRatio = float( screenWidth ) / float( screenHeight );

			switch ( projection )
			{
			case PROJECTION::PERSPECTIVE:
			{
				return Math::perspective( Math::radians( fov ), aspectRatio, near, far );
			}
			case PROJECTION::ORTHOGRAPHIC:
			{
				float top = tanf( Math::radians( fov ) * 0.5f ) * Math::distance( target, p_position );

				float bottom = -top;
				float right	 = top * aspectRatio;
				float left	 = -top * aspectRatio;

				return Math::ortho( left, right, bottom, top, near, far );
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
