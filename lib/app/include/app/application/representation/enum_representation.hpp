#ifndef __VTX_APP_APPLICATION_REPRESENTATION_ENUM_REPRESENTATION__
#define __VTX_APP_APPLICATION_REPRESENTATION_ENUM_REPRESENTATION__

#include <string>
#include <util/generic/enum_flag.hpp>
#include <vector>

namespace VTX::App::Application::Representation
{
	enum class REPRESENTATION_ENUM : int
	{
		BALL_AND_STICK,
		VAN_DER_WAALS,
		STICK,
		// !V0.1
		// TRACE,
		SAS,
		CARTOON,
		BALL_AND_STICK_AND_CARTOON,
		STICK_AND_CARTOON,
		SES,
		COUNT
	};

	inline static const std::vector<std::string> REPRESENTATION_STRING
		= { "Ball and Stick", "Van Der Waals",			"Stick" /* !V0.1, "Trace" */, "SAS",
			"Cartoon",		  "Ball Stick and Cartoon", "Stick and Cartoon",		  "SES" };

	enum class MEMBER_ENUM : int
	{
		SPHERE_RADIUS_FIXED			 = 1 << 0,
		SPHERE_RADIUS_ADD			 = 1 << 1,
		CYLINDER_RADIUS				 = 1 << 2,
		CYLINDER_COLOR_BLENDING_MODE = 1 << 3,
		RIBBON_COLOR_MODE			 = 1 << 4,
		RIBBON_COLOR_BLENDING_MODE	 = 1 << 5,
		COLOR						 = 1 << 6,
		COLOR_MODE					 = 1 << 7,
		SES_RESOLUTION				 = 1 << 8,

		NONE = 0,
		ALL	 = 0xFFFF
	};
	VTX_FLAG( MEMBER_FLAG, MEMBER_ENUM );

	enum TARGETED_DATA_ENUM : int
	{
		NONE   = 0,
		ATOM   = 1 << 0,
		BOND   = 1 << 1,
		RIBBON = 1 << 2,
		SES	   = 1 << 3,
	};
	VTX_FLAG( TARGETED_DATA_FLAG, TARGETED_DATA_ENUM );

	enum MOLECULE_COMPUTATION_ENUM : int
	{
		Targets		= 1 << 0,
		ColorBuffer = 1 << 1,

		ALL = 0xFFFF,
	};
	VTX_FLAG( MOLECULE_COMPUTATION_FLAG, MOLECULE_COMPUTATION_ENUM );

} // namespace VTX::App::Application::Representation
#endif
