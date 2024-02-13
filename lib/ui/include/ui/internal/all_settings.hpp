#ifndef __VTX_UI_INTERNAL_ALL_SETTINGS__
#define __VTX_UI_INTERNAL_ALL_SETTINGS__

#include <string>

namespace VTX::UI::Internal
{
	namespace Controller
	{
		inline static const std::string TRANSLATION_SPEED_KEY	  = "CONTROLLER_TRANSLATION_SPEED";
		inline static const float		TRANSLATION_SPEED_DEFAULT = 150.f;
		inline static const float		TRANSLATION_SPEED_MIN	  = 50.f;
		inline static const float		TRANSLATION_SPEED_MAX	  = 300.f;

		inline static const std::string ACCELERATION_FACTOR_KEY		= "ACCELERATION_FACTOR_KEY	";
		inline static const float		ACCELERATION_FACTOR_DEFAULT = 2.0f;
		inline static const float		ACCELERATION_FACTOR_MIN		= 1.0f;
		inline static const float		ACCELERATION_FACTOR_MAX		= 20.0f;
		inline static const std::string DECELERATION_FACTOR_KEY		= "DECELERATION_FACTOR_KEY	";
		inline static const float		DECELERATION_FACTOR_DEFAULT = 10.0f;
		inline static const float		DECELERATION_FACTOR_MIN		= 1.0f;
		inline static const float		DECELERATION_FACTOR_MAX		= 50.0f;

		inline static const std::string ROTATION_SPEED_KEY	   = "CONTROLLER_ROTATION_SPEED";
		inline static const float		ROTATION_SPEED_DEFAULT = 0.005f;
		inline static const float		ROTATION_SPEED_MIN	   = 0.001f;
		inline static const float		ROTATION_SPEED_MAX	   = 0.01f;

		inline static const std::string INVERT_Y_KEY	 = "INVERT_Y";
		inline static const bool		INVERT_Y_DEFAULT = false;

		inline static const std::string ELASTICITY_ACTIVE_KEY	  = "ELASTICITY_ACTIVE";
		inline static const bool		ELASTICITY_ACTIVE_DEFAULT = true;

		inline static const std::string ELASTICITY_FACTOR_KEY	  = "ELASTICITY_FACTOR";
		inline static const float		ELASTICITY_FACTOR_DEFAULT = 6.0f;
		inline static const float		ELASTICITY_FACTOR_MIN	  = 1.0f;
		inline static const float		ELASTICITY_FACTOR_MAX	  = 40.0f;
	} // namespace Controller
} // namespace VTX::UI::Internal

#endif
