#ifndef __VTX_MODEL_REPRESENTATION_ENUM__
#define __VTX_MODEL_REPRESENTATION_ENUM__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace Representation
	{
		enum class FlagDataTargeted : int
		{
			NONE   = 0,
			ATOM   = 1 << 1,
			BOND   = 1 << 2,
			RIBBON = 1 << 3,
		};

		inline FlagDataTargeted operator|( FlagDataTargeted lhs, FlagDataTargeted rhs )
		{
			return static_cast<FlagDataTargeted>( static_cast<int>( lhs ) | static_cast<int>( rhs ) );
		}
		inline FlagDataTargeted & operator|=( FlagDataTargeted & lhs, FlagDataTargeted rhs )
		{
			lhs = lhs | rhs;
			return lhs;
		}

		inline FlagDataTargeted operator&( FlagDataTargeted lhs, FlagDataTargeted rhs )
		{
			return static_cast<FlagDataTargeted>( static_cast<int>( lhs ) & static_cast<int>( rhs ) );
		}
		inline FlagDataTargeted & operator&=( FlagDataTargeted & lhs, FlagDataTargeted rhs )
		{
			lhs = lhs & rhs;
			return lhs;
		}

	} // namespace Representation
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
