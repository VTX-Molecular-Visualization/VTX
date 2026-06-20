#ifndef __VTX_TOOL_TOOLS_MDPREP_GATEWAY_SHARED__
#define __VTX_TOOL_TOOLS_MDPREP_GATEWAY_SHARED__

#include <memory>
#include <string>

namespace VTX::Tool::Mdprep::Gateway
{
	// Hold information on which item the report is about. If 'none', the report is likely to refer to an item specific
	// to the active MdEngine.
	enum class E_REPORT_CHECKED_ITEM
	{
		none,
		systemWithForceField
	};

	struct CheckReport
	{
		E_REPORT_CHECKED_ITEM itemGeneric = E_REPORT_CHECKED_ITEM::none;
		int	 itemSpecific = 0; // Item specific to the MdEngine. 0 means none. In that case, refer to the generic item.
		bool pass		  = false;
		std::string message;
		bool		dirty = false;

		bool operator==( const CheckReport & ) const noexcept = default;
	};

} // namespace VTX::Tool::Mdprep::Gateway

#endif
