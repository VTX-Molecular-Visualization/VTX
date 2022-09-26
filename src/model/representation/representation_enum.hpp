#ifndef __VTX_MODEL_REPRESENTATION_ENUM__
#define __VTX_MODEL_REPRESENTATION_ENUM__

#include "tool/logger.hpp"

namespace VTX
{
	namespace Generic
	{
		enum class REPRESENTATION : int
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
				"Cartoon",		  "Ball Stick and Cartoon", "Stick and Cartoon" };

	} // namespace Generic

	namespace Representation
	{
		enum FlagDataTargeted
		{
			NONE   = 0,
			ATOM   = 1 << 0,
			BOND   = 1 << 1,
			RIBBON = 1 << 2,
			SES	   = 1 << 3,
		};

		enum MoleculeComputationFlag
		{
			Targets			   = 1 << 0,
			ColorBuffer		   = 1 << 1,
			SecondaryStructure = 1 << 2,

			ALL = 0xFFFF,
		};

		inline static FlagDataTargeted getFlagDataTargeted( const Generic::REPRESENTATION & p_representation )
		{
			Representation::FlagDataTargeted res = Representation::FlagDataTargeted::NONE;

			switch ( p_representation )
			{
			case Generic::REPRESENTATION::VAN_DER_WAALS:
			case Generic::REPRESENTATION::SAS:
				res = Representation::FlagDataTargeted( Representation::FlagDataTargeted::ATOM );
				break;

			case Generic::REPRESENTATION::CARTOON:
				res = Representation::FlagDataTargeted( Representation::FlagDataTargeted::RIBBON );
				break;

			case Generic::REPRESENTATION::BALL_AND_STICK:
			case Generic::REPRESENTATION::STICK:
				// !V0.1
				// case Generic::REPRESENTATION::TRACE:
				res = Representation::FlagDataTargeted( Representation::FlagDataTargeted::ATOM
														| Representation::FlagDataTargeted::BOND );
				break;

			case Generic::REPRESENTATION::STICK_AND_CARTOON:
			case Generic::REPRESENTATION::BALL_AND_STICK_AND_CARTOON:
				res = Representation::FlagDataTargeted( Representation::FlagDataTargeted::ATOM
														| Representation::FlagDataTargeted::BOND
														| Representation::FlagDataTargeted::RIBBON );
				break;

			case Generic::REPRESENTATION::SES:
				res = Representation::FlagDataTargeted( Representation::FlagDataTargeted::SES );
				break;

			default:
				VTX_WARNING( "Representation " + std::to_string( int( p_representation ) )
							 + " not managed in Representation::getFlagDataTargeted" );
				break;
			}

			return res;
		}

	} // namespace Representation

} // namespace VTX
#endif
