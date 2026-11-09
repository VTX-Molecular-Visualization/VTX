//
#include "tool/mdprep/ui/md_engine.hpp"

namespace VTX::Tool::Mdprep::ui
{
	void get( const MdEngine & p_engine, Gateway::EngineSpecificCommonInformation & p_info )
	{
		if ( p_engine._ptr )
		{
			p_engine._ptr->_get( p_info );
		}
	}
} // namespace VTX::Tool::Mdprep::ui
