#ifndef __VTX_MODEL_REPRESENTATION_ENUM__
#define __VTX_MODEL_REPRESENTATION_ENUM__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace Representation
	{
		enum FlagDataTargeted
		{
			NONE   = 0,
			ATOM   = 1 << 0,
			BOND   = 1 << 1,
			RIBBON = 1 << 2,
		};

		enum MoleculeComputationFlag
		{
			Targets			   = 1 << 0,
			ColorBuffer		   = 1 << 1,
			SecondaryStructure = 1 << 2,
			ALL				   = 0xFFFF,
		};
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
			BALL_AND_STICK_AND_CARTOON,
			STICK_AND_CARTOON,
			COUNT
		};
	} // namespace Generic
} // namespace VTX
#endif
