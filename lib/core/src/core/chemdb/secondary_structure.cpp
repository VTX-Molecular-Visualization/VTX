#include "core/chemdb/secondary_structure.hpp"

namespace VTX::Core::ChemDB::SecondaryStructure
{
	TYPE pdbFormattedToEnum( const std::string & p_str )
	{
		TYPE res = TYPE::UNKNOWN;

		if ( p_str == "extended" )
			res = TYPE::STRAND;
		else if ( p_str == "turn" )
			res = TYPE::TURN;
		else if ( p_str == "coil" )
			res = TYPE::COIL;
		else if ( p_str == "right-handed alpha helix" )
			res = TYPE::HELIX_ALPHA_RIGHT;
		else if ( p_str == "left-handed alpha helix" )
			res = TYPE::HELIX_ALPHA_LEFT;
		else if ( p_str == "right-handed 3-10 helix" )
			res = TYPE::HELIX_3_10_RIGHT;
		else if ( p_str == "left-handed 3-10 helix" )
			res = TYPE::HELIX_3_10_LEFT;
		else if ( p_str == "pi helix" )
			res = TYPE::HELIX_PI;
		else if ( p_str == "right-handed omega helix" ) // ?
			res = TYPE::UNKNOWN;
		else if ( p_str == "left-handed omega helix" ) // ?
			res = TYPE::UNKNOWN;
		else if ( p_str == "right-handed gamma helix" ) // ?
			res = TYPE::UNKNOWN;
		else if ( p_str == "left-handed gamma helix" ) // ?
			res = TYPE::UNKNOWN;
		else if ( p_str == "2 - 7 ribbon / helix" ) // ?
			res = TYPE::UNKNOWN;
		else if ( p_str == "polyproline" ) // ?
			res = TYPE::UNKNOWN;

		return res;
	}

	std::string enumToPdbFormatted( const TYPE p_enum )
	{
		std::string res;

		switch ( p_enum )
		{
		case TYPE::STRAND: res = "extended"; break;
		case TYPE::TURN: res = "turn"; break;
		case TYPE::COIL: res = "coil"; break;
		case TYPE::HELIX_ALPHA_RIGHT: res = "right-handed alpha helix"; break;
		case TYPE::HELIX_ALPHA_LEFT: res = "left-handed alpha helix"; break;
		case TYPE::HELIX_3_10_RIGHT: res = "right-handed 3-10 helix"; break;
		case TYPE::HELIX_3_10_LEFT: res = "left-handed 3-10 helix"; break;
		case TYPE::HELIX_PI: res = "pi helix"; break;
		default: res = ""; break;
		}

		return res;
	}
} // namespace VTX::Core::ChemDB::SecondaryStructure
