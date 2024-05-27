#include "tools/mdprep/ui/form_data.hpp"

namespace VTX::Tool::Mdprep::ui
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

} // namespace VTX::Tool::Mdprep::ui
