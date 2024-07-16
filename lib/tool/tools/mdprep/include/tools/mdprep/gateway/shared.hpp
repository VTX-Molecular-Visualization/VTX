#ifndef __VTX_TOOL_TOOLS_MDPREP_GATEWAY_SHARED__
#define __VTX_TOOL_TOOLS_MDPREP_GATEWAY_SHARED__

#include <functional>

namespace VTX::Tool::Mdprep::Gateway
{
	struct SystemReport
	{
	};
	using SystemReportCallback = std::function<void( const SystemReport & )>;
	struct JobUpdate
	{
	};
	using JobUpdateCallback = std::function<void( const JobUpdate & )>;
} // namespace VTX::Tool::Mdprep::Gateway

#endif
