#include "tools/mdprep/gateway/form_data.hpp"

namespace VTX::Tool::Mdprep::Gateway
{
	const char * string( const E_MD_DURATION_UNIT & p_ ) noexcept
	{
		switch ( p_ )
		{
		case E_MD_DURATION_UNIT::femtoSeconds: return "fs";
		case E_MD_DURATION_UNIT::picoSeconds: return "ps";
		case E_MD_DURATION_UNIT::nanoSeconds: return "ns";
		case E_MD_DURATION_UNIT::microSeconds: return "\xc2\xb5s";

		default: return "DEBUG";
		}
	}

	const double multiplierPicoSeconds( const E_MD_DURATION_UNIT & p_ ) noexcept
	{
		switch ( p_ )
		{
		case E_MD_DURATION_UNIT::femtoSeconds: return 1e3;
		case E_MD_DURATION_UNIT::picoSeconds: return 1e0;
		case E_MD_DURATION_UNIT::nanoSeconds: return 1e3;
		case E_MD_DURATION_UNIT::microSeconds: return 1e6;
		default: return 1.0;
		}
	}

} // namespace VTX::Tool::Mdprep::Gateway
