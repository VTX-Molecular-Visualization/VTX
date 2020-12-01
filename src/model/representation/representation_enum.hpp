#ifndef __VTX_MODEL_REPRESENTATION_ENUM__
#define __VTX_MODEL_REPRESENTATION_ENUM__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace Generic
	{
		enum class REPRESENTATION : int
		{
			BALL_AND_STICK,
			VAN_DER_WAALS,
			STICK,
			TRACE,
			SAS,
			CARTOON,
			COUNT
		};
	} // namespace Generic
} // namespace VTX
#endif
